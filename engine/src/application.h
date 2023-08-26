#ifndef APPLICATION_H
#define APPLICATION_H

#include "engine.h"
#include "window/glfw/window.h"
#include "core/vulkan/swap_chain.h"
#include "core/vulkan/image_view.h"
#include "core/vulkan/render_pass.h"
#include "core/vulkan/frame_buffer.h"

#include "vulkan/vulkan.h"
#include "vector"

namespace undicht {

    class Application : public Engine {
      
      protected:

        graphics::Window _window;
        vulkan::LogicalDevice _device;
        vulkan::SwapChain _swap_chain;
        std::vector<vulkan::ImageView> _swap_images;

      public:
        
        // chooses a gpu, opens a window and sets up a swap chain
        void init(const std::string& window_title, VkPresentModeKHR present_mode);
        void cleanUp();

        void recreateSwapChain(VkPresentModeKHR present_mode);
        
        std::vector<vulkan::Framebuffer> createSwapImageFramebuffers(const vulkan::RenderPass& render_pass); // the visible attachment will be attachment 0 
        void recreateSwapImageFramebuffers(std::vector<vulkan::Framebuffer>& framebuffers, const vulkan::RenderPass& render_pass); // the visible attachment will be attachment 0 
        void destroySwapImageFramebuffers(std::vector<vulkan::Framebuffer>& framebuffers);

        graphics::Window& getWindow();
        vulkan::LogicalDevice& getDevice();
        vulkan::SwapChain& getSwapChain();
        std::vector<vulkan::ImageView>& getSwapImages();

    };

} // undicht

#endif // APPLICATION_H