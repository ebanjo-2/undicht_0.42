#include "basic_renderer.h"
#include "file_tools.h"
#include "core/vulkan/formats.h"
#include "profiler.h"

namespace undicht {

    namespace graphics {

        using namespace vulkan;

        void BasicRenderer::init(VkDevice device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_layout, VkExtent2D view_port) {

            _device_handle = device;
            _render_pass_handle = render_pass;

            initShaderModules();
            initDescriptorLayout();
            initDescriptorCache();
            initPipeLine(view_port, global_descriptor_layout, render_pass);
            initSampler();

        }

        void BasicRenderer::cleanUp() {

            _descriptor_layout.cleanUp();
            _descriptor_cache.cleanUp();
            _vertex_shader.cleanUp();
            _fragment_shader.cleanUp();
            _pipeline.cleanUp();
            _sampler.cleanUp();

        }

        void BasicRenderer::setViewPort(VkExtent2D view_port) {
            
            _pipeline.cleanUp();
            _pipeline.setViewport(view_port);
            _pipeline.init(_device_handle, _render_pass_handle);

        }

        void BasicRenderer::begin(vulkan::CommandBuffer& draw_cmd, VkDescriptorSet global_descriptor_set) {

            //_descriptor_cache.reset();
            draw_cmd.bindGraphicsPipeline(_pipeline.getPipeline());
            draw_cmd.bindDescriptorSet(global_descriptor_set, _pipeline.getPipelineLayout());

        }

        void BasicRenderer::end(vulkan::CommandBuffer& draw_cmd) {
            // just here because other renderers might need this function

        }

        uint32_t BasicRenderer::draw(vulkan::CommandBuffer& cmd, Scene& scene, Node& node) {
            /// @brief draws the meshes of the node, does not draw child nodes
            /// @return the number of draw calls that were made

            uint32_t draw_calls = 0;

            // retrieve the resources used by the node
            std::vector<Mesh*> meshes;
            for(uint32_t mesh_id : node.getMeshes())
                meshes.push_back(&scene.getMesh(mesh_id));

            // draw each mesh of the node
            for(Mesh* mesh : meshes) {

                // retrieve the meshes material
                Material& mat = scene.getMaterial(mesh->getMaterialID());
                const Texture* diff_tex = mat.getTexture(Texture::Type::DIFFUSE);

                if(!diff_tex) continue; // cant draw that mesh

                // update the descriptor set
                DescriptorSet& descriptor_set = _descriptor_cache.allocate();
                descriptor_set.bindImage(0, diff_tex->getImageView().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, _sampler.getSampler());
                descriptor_set.update();

                // bind the mesh resources
                cmd.bindDescriptorSet(descriptor_set.getDescriptorSet(), _pipeline.getPipelineLayout(), 1);
                cmd.bindVertexBuffer(mesh->getVertexBuffer().getBuffer(), 0);
                cmd.bindIndexBuffer(mesh->getIndexBuffer().getBuffer());

                // draw using the index buffer
                cmd.draw(mesh->getVertexCount(), true);
                                cmd.draw(mesh->getVertexCount(), true);

                draw_calls++;
            }

            return draw_calls;
        }

        ///////////////////////////// functions to initialize parts of the renderer /////////////////////////////

        void BasicRenderer::initShaderModules() {

            std::string directory = getFilePath(UND_CODE_SRC_FILE);

            _vertex_shader.init(_device_handle, VK_SHADER_STAGE_VERTEX_BIT, directory + "../../shader/bin/basic_shader.vert.spv");
            _fragment_shader.init(_device_handle, VK_SHADER_STAGE_FRAGMENT_BIT, directory + "../../shader/bin/basic_shader.frag.spv");

        }

        void BasicRenderer::initPipeLine(VkExtent2D view_port, VkDescriptorSetLayout global_descriptor_layout, VkRenderPass render_pass) {

            _pipeline.addShaderModule(_vertex_shader);
            _pipeline.addShaderModule(_fragment_shader);
            _pipeline.setShaderInput(global_descriptor_layout, 0);
            _pipeline.setShaderInput(_descriptor_layout.getLayout(), 1);
            _pipeline.setBlending(0, false);
            _pipeline.setDepthStencilState(true);
            _pipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
            _pipeline.setMultisampleState(false);
            _pipeline.setRasterizationState(true, VK_FRONT_FACE_CLOCKWISE);
            _pipeline.setViewport(view_port);
            _pipeline.addVertexBinding(0, 9 * sizeof(float));
            _pipeline.addVertexAttribute(0, 0, 0 * sizeof(float), translate(UND_VEC3F)); // position
            _pipeline.addVertexAttribute(0, 1, 3 * sizeof(float), translate(UND_VEC3F)); // tex coord
            _pipeline.addVertexAttribute(0, 2, 6 * sizeof(float), translate(UND_VEC3F)); // normal

            _pipeline.init(_device_handle, render_pass);

        }

        void BasicRenderer::initDescriptorLayout() {

            std::vector<VkDescriptorType> descriptor_types = {
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            };


            std::vector<VkShaderStageFlagBits> shader_stages = {
                VK_SHADER_STAGE_ALL_GRAPHICS,
            };

            _descriptor_layout.init(_device_handle, descriptor_types, shader_stages);

        }

        void BasicRenderer::initDescriptorCache() {

            // there will be a descriptor set allocted for every mesh, 5000 may or may not be enough
            _descriptor_cache.init(_device_handle, _descriptor_layout, 5000);
        }

        void BasicRenderer::initSampler() {
            
            _sampler.setMinFilter(VK_FILTER_LINEAR);
            _sampler.setMipMapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR);
            _sampler.init(_device_handle);

        }

    } // graphics

} // undicht