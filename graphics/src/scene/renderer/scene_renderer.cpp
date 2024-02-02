#include "scene_renderer.h"
#include "file_tools.h"
#include "core/vulkan/formats.h"
#include "debug.h"
#include "profiler.h"

namespace undicht {

    namespace graphics {

        using namespace vulkan;

        void SceneRenderer::init(const LogicalDevice& device, SwapChain& swap_chain, vma::VulkanMemoryAllocator& allocator) {

            _device_handle = device;
            _swap_images = swap_chain.retrieveSwapImages();

            initRenderPass(swap_chain.getSwapImageFormat());
            initDepthImages(allocator, swap_chain.getSwapImageCount(), swap_chain.getExtent());
            initFramebuffers(_swap_images, swap_chain.getExtent());
            initUniformBuffer(allocator);
            initDescriptorLayouts();
            initDescriptorCaches();
            initSampler();

            _basic_renderer.init(device.getDevice(), _render_pass.getRenderPass(), _global_descriptor_layout.getLayout(), _material_descriptor_layout.getLayout(), _node_descriptor_layout.getLayout(), swap_chain.getExtent());
            _basic_animation_renderer.init(device.getDevice(), _render_pass.getRenderPass(), _global_descriptor_layout.getLayout(), _material_descriptor_layout.getLayout(), _node_descriptor_layout.getLayout(), swap_chain.getExtent());

            _global_descriptor_set.bindUniformBuffer(0, _uniform_buffer);
            _global_descriptor_set.update();

        }

        void SceneRenderer::cleanUp(SwapChain& swap_chain) {

            _basic_renderer.cleanUp();
            _basic_animation_renderer.cleanUp();

            swap_chain.freeSwapImages(_swap_images);
            cleanUpDepthImages();
            cleanUpFramebuffers();

            _uniform_buffer.cleanUp();
            _material_sampler.cleanUp();
            _global_descriptor_layout.cleanUp();
            _global_descriptor_cache.cleanUp();
            _material_descriptor_cache.cleanUp();
            _material_descriptor_layout.cleanUp();
            _node_descriptor_layout.cleanUp();
            _node_descriptor_cache.cleanUp();
            _render_pass.cleanUp();

        }

        void SceneRenderer::recreateFramebuffers(vma::VulkanMemoryAllocator& allocator, SwapChain& swap_chain) {

            // clean up the outdated resources
            swap_chain.freeSwapImages(_swap_images);
            cleanUpDepthImages();
            cleanUpFramebuffers();

            // reinit the resources with the new swap chain extent
            _swap_images = swap_chain.retrieveSwapImages();
            initDepthImages(allocator, swap_chain.getSwapImageCount(), swap_chain.getExtent());
            initFramebuffers(_swap_images, swap_chain.getExtent());

            // change the viewport of the renderers
            _basic_renderer.setViewPort(swap_chain.getExtent());
            _basic_animation_renderer.setViewPort(swap_chain.getExtent());

        }

        void SceneRenderer::loadCameraMatrices(float* mat4_view, float* mat4_proj, TransferBuffer& transfer_buffer) {

            _uniform_buffer.uploadData(0, (const uint8_t*)mat4_view, transfer_buffer);
            _uniform_buffer.uploadData(1, (const uint8_t*)mat4_proj, transfer_buffer);

        }

        ////////////////////////////////////// drawing //////////////////////////////////////

        void SceneRenderer::begin(vulkan::CommandBuffer& cmd, uint32_t swap_image_id) {
            
            VkClearValue clear_color = {0.2f, 0.2f, 0.2f, 0.0f};
            VkClearValue clear_depth = {1.0f, 0.0f};

            cmd.beginRenderPass(_render_pass.getRenderPass(), _framebuffers.at(swap_image_id).getFramebuffer(), _framebuffers.at(swap_image_id).getExtent(), {clear_color, clear_depth});

        }

        void SceneRenderer::end(vulkan::CommandBuffer& cmd) {

            cmd.endRenderPass();
        }

        uint32_t SceneRenderer::draw(vulkan::CommandBuffer& cmd, Scene& scene) {
            /// @return the number of draw calls that were made

            // counting draw calls
            uint32_t draw_calls = 0;

            // draw all meshes that dont have skeletal animation
            _basic_renderer.begin(cmd, _global_descriptor_set.getDescriptorSet());
            draw_calls += drawStatic(cmd, scene, scene.getRootNode());
            _basic_renderer.end(cmd);

            // draw all meshes that do have skeletal animation
            _basic_animation_renderer.begin(cmd, _global_descriptor_set.getDescriptorSet());
            draw_calls += drawAnimated(cmd, scene, scene.getRootNode());
            _basic_animation_renderer.end(cmd);

            return draw_calls;
        }

        uint32_t SceneRenderer::drawStatic(vulkan::CommandBuffer& cmd, Scene& scene, Node& node) {
            /// @return the number of draw calls that were made

            // doesnt draw the nodes children
            uint32_t draw_calls = _basic_renderer.draw(cmd, scene, node);

            // recursivly draw the child nodes
            for(Node& n : node.getChildNodes()) {
                draw_calls += drawStatic(cmd, scene, n);
            }

            return draw_calls;
        }

        uint32_t SceneRenderer::drawAnimated(vulkan::CommandBuffer& cmd, Scene& scene, Node& node) {
            /// @return the number of draw calls that were made

            // doesnt draw the nodes children
            uint32_t draw_calls = _basic_animation_renderer.draw(cmd, scene, node);

            // recursivly draw the child nodes
            for(Node& n : node.getChildNodes()) {
                draw_calls += drawAnimated(cmd, scene, n);
            }

            return draw_calls;
        }

        vulkan::DescriptorSetCache& SceneRenderer::getMaterialDescriptorCache() {

            return _material_descriptor_cache;
        }

        vulkan::DescriptorSetCache& SceneRenderer::getNodeDescriptorCache() {

            return _node_descriptor_cache;
        }

        vulkan::Sampler& SceneRenderer::getMaterialSampler() {

            return _material_sampler;
        }

        /////////////////////////////// non public renderer functions ///////////////////////

        void SceneRenderer::initRenderPass(VkFormat swap_image_format) {
            
            _render_pass.addAttachment(swap_image_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            _render_pass.addAttachment(translate(UND_DEPTH32F), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
            _render_pass.addSubPass({0, 1}, {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL});
            _render_pass.init(_device_handle.getDevice());
        }

        void SceneRenderer::initDepthImages(vma::VulkanMemoryAllocator& allocator, uint32_t image_count, const VkExtent2D &extent) {

            _depth_images.resize(image_count);
            _depth_image_views.resize(image_count);

            for(int i = 0; i < image_count; i++) {

                VkImageUsageFlags image_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                VmaAllocationCreateFlags alloc_flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
                _depth_images.at(i).init(allocator, translate(UND_DEPTH32F), {extent.width, extent.height, 1}, image_usage, VMA_MEMORY_USAGE_GPU_ONLY, alloc_flags);
                _depth_image_views.at(i).init(_device_handle.getDevice(), _depth_images.at(i).getImage(), translate(UND_DEPTH32F));

            }

        }

        void SceneRenderer::initFramebuffers(std::vector<ImageView> swap_images, const VkExtent2D& extent) {

            _framebuffers.resize(swap_images.size());
            for(int i = 0; i < swap_images.size(); i++) {
                _framebuffers.at(i).setAttachment(0, swap_images.at(i).getImageView());
                _framebuffers.at(i).setAttachment(1, _depth_image_views.at(i).getImageView());
                _framebuffers.at(i).init(_device_handle.getDevice(), _render_pass, extent);
            }

        }

        void SceneRenderer::initDescriptorLayouts() {

            std::vector<VkDescriptorType> global_descriptors = {
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            };

            std::vector<VkDescriptorType> material_descriptors = {
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            };

            std::vector<VkDescriptorType> node_descriptors = {
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            };

            std::vector<VkShaderStageFlagBits> shader_stages = {
                VK_SHADER_STAGE_ALL_GRAPHICS,
            };

            _global_descriptor_layout.init(_device_handle.getDevice(), global_descriptors, shader_stages);
            _node_descriptor_layout.init(_device_handle.getDevice(), node_descriptors, shader_stages);
            _material_descriptor_layout.init(_device_handle.getDevice(), material_descriptors, shader_stages);

        }

        void SceneRenderer::initDescriptorCaches() {

            // descriptor to bind the global uniform buffer
            _global_descriptor_cache.init(_device_handle.getDevice(), _global_descriptor_layout, 1);
            _global_descriptor_set = _global_descriptor_cache.allocate();

            // there will be a descriptor set allocted for every material, 500 may or may not be enough
            _material_descriptor_cache.init(_device_handle.getDevice(), _material_descriptor_layout, 500);

            // there will be a descriptor set allocted for every node, 5000 may or may not be enough
            _node_descriptor_cache.init(_device_handle.getDevice(), _node_descriptor_layout, 5000);
        }

        void SceneRenderer::initUniformBuffer(vma::VulkanMemoryAllocator& allocator) {

            // layout of the uniform buffer
            // 0: matrix 4x4 : camera view matrix
            // 1: matrix 4x4 : camera projetion matrix
            const BufferLayout ubo_layout({UND_MAT4F, UND_MAT4F});

            _uniform_buffer.init(_device_handle, allocator, ubo_layout);

        }

        void SceneRenderer::initSampler() {
            
            _material_sampler.setMinFilter(VK_FILTER_NEAREST);
            _material_sampler.setMipMapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR);
            _material_sampler.init(_device_handle.getDevice());

        }

        void SceneRenderer::cleanUpFramebuffers() {

            for(Framebuffer& f : _framebuffers) f.cleanUp();
        }

        void SceneRenderer::cleanUpDepthImages() {

            for(Image& i : _depth_images) i.cleanUp();
            for(ImageView& i : _depth_image_views) i.cleanUp();

        }

    } // graphics

} // undicht