#ifndef BASIC_APP_TEMPLATE_H
#define BASIC_APP_TEMPLATE_H

#include "application.h"
#include "vulkan_memory_allocator.h"
#include "frame_manager.h"
#include "core/vulkan/image.h"

namespace undicht {

    class BasicAppTemplate : public Application, public FrameManager {
        /** implements some functionality that is commonly used when creating basic applications 
         * to use this functionality simply inherit it from this class 
         * or copy the code to your own class if you want to make modifications
         * some of the functions of this class are virtual so that they may be overwritten */
      protected:
        
        // The Vulkan® Memory Allocator (VMA) library provides a simple and easy to integrate API
        // to help you allocate memory for Vulkan® buffer and image storage
        // https://gpuopen.com/vulkan-memory-allocator/
        vma::VulkanMemoryAllocator _vulkan_allocator;

        // resources to render to a visible attachment of the swap chain
        vulkan::RenderPass _default_render_pass;
        std::vector<vulkan::ImageView> _visible_swap_images;
        std::vector<vulkan::Image> _default_depth_images;
        std::vector<vulkan::ImageView> _default_depth_image_views;
        std::vector<vulkan::Framebuffer> _default_framebuffers;

        // storing settings
        VkPresentModeKHR _present_mode;
        VkClearValue _clear_color = {0.2f, 0.2f, 0.2f, 0.0f};
        VkClearValue _clear_depth = {1.0f, 0.0f};

      public:

        void init(const std::string& window_title, VkPresentModeKHR present_mode);
        void cleanUp();

        /// @brief main loop function of the application
        /// @return true, as long as the application wants to run (use it like this: while(app.run()); )
        bool run();

      protected:
        // internal functions (may be overwritten)

        // settings
        void setClearColor(VkClearValue clear_color);
        void setDepthClearValue(VkClearValue clear_depth);

        // setup functions
        virtual void setupVulkanAllocator();
        virtual void setupDefaultRenderPass();
        virtual void setupVisibleSwapImages();
        virtual void setupDepthImages();
        virtual void setupDepthImageViews();
        virtual void setupDefaultFramebuffers();

        // functions for building a frame
        virtual void framePreperation(); // called while the old frame is still being processed on the gpu
        virtual void transferCommands(); // called after the previous frames transfer commands have finished
        virtual void drawCommands(int swap_image_id); // record draw commands here
        virtual void frameFinalization(); // (run must be called with wait_for_previous_frame = true) called after the previous frame has finished rendering
        virtual void recreateSwapChain();

        // helper functions
        void beginDefaultRenderPass(int swap_image_id);
        void endDefaultRenderPass();

    };

} // undicht

#endif // BASIC_APP_TEMPLATE_H