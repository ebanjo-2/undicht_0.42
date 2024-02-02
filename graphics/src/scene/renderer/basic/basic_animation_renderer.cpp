#include "basic_animation_renderer.h"
#include "file_tools.h"
#include "core/vulkan/formats.h"
#include "profiler.h"
#include "debug.h"

namespace undicht {

    namespace graphics {

        using namespace vulkan;

        void BasicAnimationRenderer::init(VkDevice device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_layout, VkDescriptorSetLayout material_descriptor_layout, VkDescriptorSetLayout node_descriptor_layout, VkExtent2D view_port) {
            
            BasicRendererTemplate::init(device, render_pass, view_port);

            _global_descriptor_layout = global_descriptor_layout;
            _material_descriptor_layout = material_descriptor_layout;
            _node_descriptor_layout = node_descriptor_layout;

            initShaderModules();
            initPipeLine(view_port);

        }

        void BasicAnimationRenderer::cleanUp() {

            BasicRendererTemplate::cleanUp();
        }

        void BasicAnimationRenderer::begin(vulkan::CommandBuffer& draw_cmd, VkDescriptorSet global_descriptor_set) {

            BasicRendererTemplate::begin(draw_cmd);
            
            draw_cmd.bindDescriptorSet(global_descriptor_set, _pipeline.getPipelineLayout());

        }

        uint32_t BasicAnimationRenderer::draw(vulkan::CommandBuffer& cmd, Scene& scene, Node& node) {
            /// @brief draws the meshes of the node, does not draw child nodes
            /// @return the number of draw calls that were made

            uint32_t draw_calls = 0;

            // retrieve the resources used by the node
            std::vector<Mesh*> meshes;
            for(uint32_t mesh_id : node.getMeshes())
                meshes.push_back(&scene.getMesh(mesh_id));

            // draw each mesh of the node
            for(Mesh* mesh : meshes) {

                if(!mesh->getHasBones()) continue; // this renderer can only draw meshes with skeletal animation

                // retrieve the meshes material
                Material& mat = scene.getMaterial(mesh->getMaterialID());

                if(!mat.getTexture(Texture::Type::DIFFUSE)) continue; // cant draw that mesh

                // bind the mesh resources
                cmd.bindDescriptorSet(mat.getDescriptorSet().getDescriptorSet(), _pipeline.getPipelineLayout(), 1);
                cmd.bindDescriptorSet(node.getDescriptorSet().getDescriptorSet(), _pipeline.getPipelineLayout(), 2);
                cmd.bindVertexBuffer(mesh->getVertexBuffer().getBuffer(), 0);
                cmd.bindIndexBuffer(mesh->getIndexBuffer().getBuffer());

                // draw using the index buffer
                cmd.draw(mesh->getVertexCount(), true);

                draw_calls++;
            }

            return draw_calls;
        }

        ///////////////////////////// functions to initialize parts of the renderer /////////////////////////////

        void BasicAnimationRenderer::initShaderModules() {

            std::string directory = getFilePath(UND_CODE_SRC_FILE);

            _vertex_shader.init(_device_handle, VK_SHADER_STAGE_VERTEX_BIT, directory + "../../shader/bin/basic_animation_shader.vert.spv");
            _fragment_shader.init(_device_handle, VK_SHADER_STAGE_FRAGMENT_BIT, directory + "../../shader/bin/basic_animation_shader.frag.spv");

        }

        //////////////////////////// functions that initialize parts of the pipeline /////////////////////////////

        void BasicAnimationRenderer::setShaderInput() {

            _pipeline.setShaderInput(_global_descriptor_layout, 0);
            _pipeline.setShaderInput(_material_descriptor_layout, 1);
            _pipeline.setShaderInput(_node_descriptor_layout, 2);

        }

        void BasicAnimationRenderer::setVertexAttributes() {

            _pipeline.addVertexBinding(0, 17 * sizeof(float));
            _pipeline.addVertexAttribute(0, 0, 0 * sizeof(float), translate(UND_VEC3F)); // position
            _pipeline.addVertexAttribute(0, 1, 3 * sizeof(float), translate(UND_VEC3F)); // tex coord
            _pipeline.addVertexAttribute(0, 2, 6 * sizeof(float), translate(UND_VEC3F)); // normal
            _pipeline.addVertexAttribute(0, 3, 9 * sizeof(float), translate(UND_VEC4I)); // bone ids
            _pipeline.addVertexAttribute(0, 4, 13 * sizeof(float), translate(UND_VEC4F)); // bone weights
            _pipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        }

        void BasicAnimationRenderer::setRasterizationState() {

            _pipeline.setRasterizationState(false, VK_FRONT_FACE_CLOCKWISE);
        }

        void BasicAnimationRenderer::addShaderModules() {

            _pipeline.addShaderModule(_vertex_shader);
            _pipeline.addShaderModule(_fragment_shader);
        }

        void BasicAnimationRenderer::setBlending() {

            _pipeline.setBlending(0, false);
        }

        void BasicAnimationRenderer::setDepthStencilState() {

            _pipeline.setDepthStencilState(true, true, VK_COMPARE_OP_LESS, false);
        }

        void BasicAnimationRenderer::setMultisampleState() {

            _pipeline.setMultisampleState(false);
        }

    } // graphics

} // undicht