#include "basic_renderer.h"
#include "file_tools.h"
#include "core/vulkan/formats.h"
#include "profiler.h"
#include "debug.h"

namespace undicht {

    namespace graphics {

        using namespace vulkan;

        void BasicRenderer::init(VkDevice device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_layout, VkDescriptorSetLayout material_descriptor_layout, VkDescriptorSetLayout node_descriptor_layout, VkExtent2D view_port) {
            
            BasicRendererTemplate::init(device, render_pass, view_port);

            _global_descriptor_layout = global_descriptor_layout;
            _material_descriptor_layout = material_descriptor_layout;
            _node_descriptor_layout = node_descriptor_layout;

            initShaderModules();
            initPipeLine(view_port);

        }

        void BasicRenderer::cleanUp() {

            BasicRendererTemplate::cleanUp();
        }

        void BasicRenderer::begin(vulkan::CommandBuffer& draw_cmd, VkDescriptorSet global_descriptor_set) {

            BasicRendererTemplate::begin(draw_cmd);
            
            draw_cmd.bindDescriptorSet(global_descriptor_set, _pipeline.getPipelineLayout());

        }

        uint32_t BasicRenderer::draw(vulkan::CommandBuffer& cmd, SceneGroup& scene, Node& node) {
            /// @brief draws the meshes of the node, does not draw child nodes
            /// @return the number of draw calls that were made

            // retrieve the resources used by the node
            // this renderer cant draw meshes with skeletal animation
            Mesh* mesh = node.getMesh(scene);
            if(!mesh || mesh->getHasBones()) return 0;

            // retrieve the meshes material
            Material* mat = mesh->getMaterial(scene);
            if(!mat) return 0;
            if(!mat->getTexture(Texture::Type::DIFFUSE)) return 0; // cant draw that mesh

            // bind the mesh resources
            cmd.bindDescriptorSet(mat->getDescriptorSet().getDescriptorSet(), _pipeline.getPipelineLayout(), 1);
            cmd.bindDescriptorSet(node.getDescriptorSet().getDescriptorSet(), _pipeline.getPipelineLayout(), 2);
            cmd.bindVertexBuffer(mesh->getVertexBuffer().getBuffer(), 0);
            cmd.bindIndexBuffer(mesh->getIndexBuffer().getBuffer());

            // draw using the index buffer
            cmd.draw(mesh->getVertexCount(), true);

            return 1;
        }

        ///////////////////////////// functions to initialize parts of the renderer /////////////////////////////

        void BasicRenderer::initShaderModules() {

            std::string directory = getFilePath(UND_CODE_SRC_FILE);

            _vertex_shader.init(_device_handle, VK_SHADER_STAGE_VERTEX_BIT, directory + "../../shader/bin/basic_shader.vert.spv");
            _fragment_shader.init(_device_handle, VK_SHADER_STAGE_FRAGMENT_BIT, directory + "../../shader/bin/basic_shader.frag.spv");

        }

        //////////////////////////// functions that initialize parts of the pipeline /////////////////////////////

        void BasicRenderer::setShaderInput() {

            _pipeline.setShaderInput(_global_descriptor_layout, 0);
            _pipeline.setShaderInput(_material_descriptor_layout, 1);
            _pipeline.setShaderInput(_node_descriptor_layout, 2);

        }

        void BasicRenderer::setVertexAttributes() {

            _pipeline.addVertexBinding(0, 9 * sizeof(float));
            _pipeline.addVertexAttribute(0, 0, 0 * sizeof(float), translate(UND_VEC3F)); // position
            _pipeline.addVertexAttribute(0, 1, 3 * sizeof(float), translate(UND_VEC3F)); // tex coord
            _pipeline.addVertexAttribute(0, 2, 6 * sizeof(float), translate(UND_VEC3F)); // normal
            _pipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        }

        void BasicRenderer::setRasterizationState() {

            _pipeline.setRasterizationState(false, VK_FRONT_FACE_CLOCKWISE);
        }

        void BasicRenderer::addShaderModules() {

            _pipeline.addShaderModule(_vertex_shader);
            _pipeline.addShaderModule(_fragment_shader);
        }

        void BasicRenderer::setBlending() {

            _pipeline.setBlending(0, false);
        }

        void BasicRenderer::setDepthStencilState() {

            _pipeline.setDepthStencilState(true, true, VK_COMPARE_OP_LESS, false);
        }

        void BasicRenderer::setMultisampleState() {

            _pipeline.setMultisampleState(false);
        }

    } // graphics

} // undicht