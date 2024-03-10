#include "basic_app_template.h"
#include "types.h"
#include "core/vulkan/formats.h"

namespace undicht {

    using namespace vulkan;

    void BasicAppTemplate::init(const std::string& window_title, VkPresentModeKHR present_mode) {

        _present_mode = present_mode;

        Application::init(window_title, present_mode);
        FrameManager::init(getDevice());

        setupVulkanAllocator();
        setupDefaultRenderPass();
        setupVisibleSwapImages();
        setupDepthImages();
        setupDepthImageViews();
        setupDefaultFramebuffers();

    }

    void BasicAppTemplate::cleanUp() {

        getDevice().waitForProcessesToFinish();

        for(Framebuffer& f : _default_framebuffers) f.cleanUp();
        for(ImageView& i : _visible_swap_images) i.cleanUp();
        for(Image& i : _default_depth_images) i.cleanUp();
        for(ImageView& i : _default_depth_image_views) i.cleanUp();
        _default_render_pass.cleanUp();
        _vulkan_allocator.cleanUp();

        FrameManager::cleanUp();
        Application::cleanUp();
    }

    bool BasicAppTemplate::run() {
        /// @brief main loop function of the application
        /// @param wait_for_previous_frame only if true the application will wait for the previous frame 
        //   to finish before submitting the next one
        /// @return true, as long as the application wants to run (use it like this: while(app.run()); )

        // the structure of a frame should look like this:

        // Step 1: run code that doesnt use gpu resources while the old frame is still processed on the gpu
        framePreperation();

        // Step 2: accqire a new swap image
        uint32_t swap_image_id = FrameManager::prepareNextFrame(_swap_chain);

        // Step 3: wait for the transfer operations of the previous frame to finish
        // in order to be able to reuse the resources for transferring to / from the gpu
        FrameManager::waitForTransferFinished();

        if(swap_image_id != -1) {

            // Step 4: reset & begin the command buffers (yes, every frame)
            getTransferCmd().resetCommandBuffer();
            getTransferCmd().beginCommandBuffer(true);
            getDrawCmd().resetCommandBuffer();
            getDrawCmd().beginCommandBuffer(true);

            // Step 5: record transfer commands
            transferCommands();

            // Step 6: record draw commands
            drawCommands(swap_image_id);

            // Step 7: end command buffers
            getTransferCmd().endCommandBuffer();
            getDrawCmd().endCommandBuffer();

            // Step 8: wait for previous frame to finish
            FrameManager::finishLastFrame();
            frameFinalization();

            // Step 9: submit current frame to the queue
            FrameManager::submitFrame(getDevice(), _swap_chain, true);

        } else {

            recreateSwapChain();
            FrameManager::reset();
        }


        getWindow().update();
        return !getWindow().shouldClose();
    }

    ///////////////////////////////// internal functions (may be overwritten) /////////////////////////////////
    //////////////////////////////////////////////// settings /////////////////////////////////////////////////
        
    void BasicAppTemplate::setClearColor(VkClearValue clear_color) {

        _clear_color = clear_color;
    }

    void BasicAppTemplate::setDepthClearValue(VkClearValue clear_depth) {

        _clear_depth = clear_depth;
    }

    ///////////////////////////////////////////// setup functions /////////////////////////////////////////////

    void BasicAppTemplate::setupVulkanAllocator() {

        _vulkan_allocator.init(getVulkanInstance().getInstance(), getDevice().getDevice(), getDevice().getPhysicalDevice());
    }

    void BasicAppTemplate::setupDefaultRenderPass() { 
        /// creates a simple renderpass that has one visible color attachment

        _default_render_pass.addAttachment(getSwapChain().getSwapImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        _default_render_pass.addAttachment(translate(UND_DEPTH32F), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        _default_render_pass.addSubPass({0, 1}, {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL});
        _default_render_pass.init(getDevice().getDevice());
    }

    void BasicAppTemplate::setupVisibleSwapImages() {

        _visible_swap_images = _swap_chain.retrieveSwapImages();
    }

    void BasicAppTemplate::setupDepthImages() {

        int image_count = _swap_chain.getSwapImageCount();
        VkExtent3D extent = {_swap_chain.getExtent().width, _swap_chain.getExtent().height, 1};

        _default_depth_images.resize(image_count);

        for(Image& depth_image : _default_depth_images) {

            VkImageUsageFlags image_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            VmaAllocationCreateFlags alloc_flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
            depth_image.init(_vulkan_allocator, translate(UND_DEPTH32F), extent, image_usage, VMA_MEMORY_USAGE_GPU_ONLY, alloc_flags);

        }

    }

    void BasicAppTemplate::setupDepthImageViews() {

        _default_depth_image_views.resize(_default_depth_images.size());

        for(int i = 0; i < _default_depth_images.size(); i++) {

            Image& depth_image = _default_depth_images.at(i);
            ImageView& depth_image_view = _default_depth_image_views.at(i);

            depth_image_view.init(getDevice().getDevice(), depth_image.getImage(), translate(UND_DEPTH32F));
        }

    }

    void BasicAppTemplate::setupDefaultFramebuffers() {

        _default_framebuffers.resize(_visible_swap_images.size());

        for(int i = 0; i < _visible_swap_images.size(); i++) {

            _default_framebuffers.at(i).setAttachment(0, _visible_swap_images.at(i).getImageView());
            _default_framebuffers.at(i).setAttachment(1, _default_depth_image_views.at(i).getImageView());
            _default_framebuffers.at(i).init(getDevice().getDevice(), _default_render_pass, _swap_chain.getExtent());

        }

    }

    ///////////////////////////////////// functions for building a frame ////////////////////////////////////

    void BasicAppTemplate::framePreperation() {
        // executed before the old frame is finished on the gpu

    } 

    void BasicAppTemplate::transferCommands() {
        // executed after the previous frames transfer commands have finished

    } 
    
    void BasicAppTemplate::drawCommands(int swap_image_id) {
        // record draw commands here

    }

    void BasicAppTemplate::frameFinalization() {
        // called after the previous frame has finished rendering

    }
    
    void BasicAppTemplate::recreateSwapChain() {

        // reset semaphores
        _swap_images_ready[_frame_id].reset();
        
        // recreate the swap chain
        Application::recreateSwapChain(_present_mode);

        // update the resources using the swap chain
        // Step 1: clean up the outdated resources
        _swap_chain.freeSwapImages(_visible_swap_images);
        for(Framebuffer& f : _default_framebuffers) f.cleanUp();
        for(Image& i : _default_depth_images) i.cleanUp();
        for(ImageView& i : _default_depth_image_views) i.cleanUp();
        
        // Step 2: reinit the resources with the new swap chain extent
        setupVisibleSwapImages();
        setupDepthImages();
        setupDepthImageViews();
        setupDefaultFramebuffers();

    }        
    
    //////////////////////////////// helper functions ////////////////////////////////

    void BasicAppTemplate::beginDefaultRenderPass(int swap_image_id) {

        getDrawCmd().beginRenderPass(_default_render_pass.getRenderPass(), _default_framebuffers.at(swap_image_id).getFramebuffer(), _swap_chain.getExtent(), {_clear_color, _clear_color});
    }

    void BasicAppTemplate::endDefaultRenderPass() {

        getDrawCmd().endRenderPass();
    }
    
} // undicht