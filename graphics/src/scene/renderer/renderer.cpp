#include "renderer.h"
#include "file_tools.h"
#include "core/vulkan/formats.h"
#include "debug.h"

namespace undicht {

    using namespace vulkan;
    using namespace tools;

    void Renderer::init(const LogicalDevice& device, SwapChain& swap_chain, vma::VulkanMemoryAllocator& allocator) {

        _device_handle = device;
        _swap_images = swap_chain.retrieveSwapImages();

        initShaderModules();
        initRenderPass(swap_chain.getSwapImageFormat());
        initDepthImages(allocator, swap_chain.getSwapImageCount(), swap_chain.getExtent());
        initFramebuffers(_swap_images, swap_chain.getExtent());

        initDescriptorLayouts();
        initDescriptorCaches();
        initPipeline(swap_chain.getExtent());

        initUniformBuffer(allocator);
        initSampler();

    }

    void Renderer::cleanUp(SwapChain& swap_chain) {

        swap_chain.freeSwapImages(_swap_images);
        cleanUpDepthImages();
        cleanUpFramebuffers();

        _uniform_buffer.cleanUp();
        _sampler.cleanUp();

        _global_descriptor_layout.cleanUp();
        _global_descriptor_cache.cleanUp();
        _per_node_descriptor_layout.cleanUp();
        _per_node_descriptor_cache.cleanUp();
        _render_pass.cleanUp();
        _pipeline.cleanUp();
        _vertex_shader.cleanUp();
        _fragment_shader.cleanUp();

    }

    void Renderer::recreateFramebuffers(vma::VulkanMemoryAllocator& allocator, SwapChain& swap_chain) {

        // clean up the outdated resources
        swap_chain.freeSwapImages(_swap_images);
        cleanUpDepthImages();
        cleanUpFramebuffers();

        // reinit the resources with the new swap chain extent
        _swap_images = swap_chain.retrieveSwapImages();
        initDepthImages(allocator, swap_chain.getSwapImageCount(), swap_chain.getExtent());
        initFramebuffers(_swap_images, swap_chain.getExtent());

        // change the viewport
        _pipeline.cleanUp();
        _pipeline.setViewport(swap_chain.getExtent());
        _pipeline.init(_device_handle.getDevice(), _render_pass.getRenderPass());

    }

    void Renderer::loadCameraMatrices(float* mat4_view, float* mat4_proj) {

        _uniform_buffer.setData(0, mat4_view);
        _uniform_buffer.setData(1, mat4_proj);

    }

    ////////////////////////////////////// drawing //////////////////////////////////////

    void Renderer::begin(vulkan::CommandBuffer& cmd, vulkan::Semaphore& swap_image_ready, uint32_t swap_image_id) {
        
        VkClearValue clear_color = {0.2f, 0.2f, 0.2f, 0.0f};
        VkClearValue clear_depth = {1.0f, 0.0f};

        cmd.beginRenderPass(_render_pass.getRenderPass(), _framebuffers.at(swap_image_id).getFramebuffer(), _framebuffers.at(swap_image_id).getExtent(), {clear_color, clear_depth});
        cmd.bindGraphicsPipeline(_pipeline.getPipeline());

        _global_descriptor_cache.reset();
        _per_node_descriptor_cache.reset();

        // bind camera ubo
        DescriptorSet& descriptor_set = _global_descriptor_cache.allocate();
        descriptor_set.bindUniformBuffer(0, _uniform_buffer);
        descriptor_set.update();
        cmd.bindDescriptorSet(descriptor_set.getDescriptorSet(), _pipeline.getPipelineLayout());

    }

    void Renderer::end(vulkan::CommandBuffer& cmd) {

        cmd.endRenderPass();
    }

    void Renderer::draw(vulkan::CommandBuffer& cmd, Scene& scene) {

        // will draw the entire scene recursivly
        draw(cmd, scene, scene.getRootNode());

    }

    void Renderer::draw(vulkan::CommandBuffer& cmd, Scene& scene, Node& node) {

        // retrieve the resources used by the node
        std::vector<Mesh*> meshes;
        for(uint32_t mesh_id : node.getMeshes())
            meshes.push_back(&scene.getMesh(mesh_id));

        // draw each mesh of the node
        for(Mesh* mesh : meshes) {

            // retrieve the meshes material
            Material& mat = scene.getMaterial(mesh->getMaterialID());
            const Texture* dif_tex = mat.getTexture(Texture::Type::DIFFUSE);

            // bind the resources for the current mesh
            DescriptorSet& descriptor_set = _per_node_descriptor_cache.allocate();

            if(dif_tex) {

                descriptor_set.bindImage(0, dif_tex->getImageView().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, _sampler.getSampler());
            } else {

                // UND_WARNING << "mesh " << mesh->getName() << " doesnt use a diffuse texture\n";
            }

            descriptor_set.update();
            cmd.bindDescriptorSet(descriptor_set.getDescriptorSet(), _pipeline.getPipelineLayout(), 1);

            cmd.bindVertexBuffer(mesh->getVertexBuffer().getBuffer(), 0);
            cmd.bindIndexBuffer(mesh->getIndexBuffer().getBuffer());

            // draw using the index buffer
            cmd.draw(mesh->getVertexCount(), true);

        }

        // recursivly draw the child nodes
        for(Node& n : node.getChildNodes()) {
            draw(cmd, scene, n);
        }

    }

    /////////////////////////////// non public renderer functions ///////////////////////

    void Renderer::initShaderModules() {

        std::string directory = getFilePath(UND_CODE_SRC_FILE);

        _vertex_shader.init(_device_handle.getDevice(), VK_SHADER_STAGE_VERTEX_BIT, directory + "../shader/bin/cube.vert.spv");
        _fragment_shader.init(_device_handle.getDevice(), VK_SHADER_STAGE_FRAGMENT_BIT, directory + "../shader/bin/cube.frag.spv");

    }

    void Renderer::initRenderPass(VkFormat swap_image_format) {
        
        _render_pass.addAttachment(swap_image_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        _render_pass.addAttachment(translate(UND_DEPTH32F), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        _render_pass.addSubPass({0, 1}, {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL});
        _render_pass.init(_device_handle.getDevice());
    }

    void Renderer::initDepthImages(vma::VulkanMemoryAllocator& allocator, uint32_t image_count, const VkExtent2D &extent) {

        _depth_images.resize(image_count);
        _depth_image_views.resize(image_count);

        for(int i = 0; i < image_count; i++) {
            
            _depth_images.at(i).init(allocator, translate(UND_DEPTH32F), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, {extent.width, extent.height, 1});
            _depth_image_views.at(i).init(_device_handle.getDevice(), _depth_images.at(i).getImage(), translate(UND_DEPTH32F));

        }

    }

    void Renderer::initFramebuffers(std::vector<ImageView> swap_images, const VkExtent2D& extent) {

        _framebuffers.resize(swap_images.size());
        for(int i = 0; i < swap_images.size(); i++) {
            _framebuffers.at(i).setAttachment(0, swap_images.at(i).getImageView());
            _framebuffers.at(i).setAttachment(1, _depth_image_views.at(i).getImageView());
            _framebuffers.at(i).init(_device_handle.getDevice(), _render_pass, extent);
        }

    }

    void Renderer::initPipeline(const VkExtent2D& view_port) {

        _pipeline.addShaderModule(_vertex_shader);
        _pipeline.addShaderModule(_fragment_shader);
        _pipeline.setShaderInput(_global_descriptor_layout.getLayout(), 0);
        _pipeline.setShaderInput(_per_node_descriptor_layout.getLayout(), 1);
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

        _pipeline.init(_device_handle.getDevice(), _render_pass.getRenderPass());

    }

    void Renderer::initDescriptorLayouts() {

        std::vector<VkDescriptorType> global_descriptors = {
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        };

        std::vector<VkDescriptorType> per_node_descriptors = {
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        };

        std::vector<VkShaderStageFlagBits> shader_stages = {
            VK_SHADER_STAGE_ALL_GRAPHICS,
        };

        _global_descriptor_layout.init(_device_handle.getDevice(), global_descriptors, shader_stages);
        _per_node_descriptor_layout.init(_device_handle.getDevice(), per_node_descriptors, shader_stages);

    }

    void Renderer::initDescriptorCaches() {

        _global_descriptor_cache.init(_device_handle.getDevice(), _global_descriptor_layout, 3);
        _per_node_descriptor_cache.init(_device_handle.getDevice(), _per_node_descriptor_layout, 5000);

    }

    void Renderer::initUniformBuffer(vma::VulkanMemoryAllocator& allocator) {

        // layout of the uniform buffer
        // 0: matrix 4x4 : camera view matrix
        // 1: matrix 4x4 : camera projetion matrix
        const BufferLayout ubo_layout({UND_MAT4F, UND_MAT4F});

        _uniform_buffer.init(_device_handle, allocator, ubo_layout);

    }

    void Renderer::initSampler() {

        _sampler.init(_device_handle.getDevice());
    }

    void Renderer::cleanUpFramebuffers() {

        for(Framebuffer& f : _framebuffers) f.cleanUp();
    }

    void Renderer::cleanUpDepthImages() {

        for(Image& i : _depth_images) i.cleanUp();
        for(ImageView& i : _depth_image_views) i.cleanUp();

    }

} // undicht