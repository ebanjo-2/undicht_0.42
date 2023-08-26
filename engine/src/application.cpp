#include "application.h"


namespace undicht {

    // chooses a gpu, opens a window and sets up a swap chain
    void Application::init(const std::string& window_title, VkPresentModeKHR present_mode) {

        Engine::init();

        _window.open(getVulkanInstance().getInstance(), window_title);
        _device.init(getVulkanInstance().chooseGPU(), _window.getSurface());
        _swap_chain.init(_device, _window.getSurface(), present_mode);
        _swap_images = _swap_chain.retrieveSwapImages();

    }

    void Application::cleanUp() {

        _swap_chain.freeSwapImages(_swap_images);
        _swap_chain.cleanUp();
        _device.cleanUp();
        _window.close();
        Engine::cleanUp();
    }

    void Application::recreateSwapChain(VkPresentModeKHR present_mode) {

        _swap_chain.recreate(_device, _window.getSurface(),  present_mode);
        _swap_chain.freeSwapImages(_swap_images);
        _swap_images = _swap_chain.retrieveSwapImages();

    }

    std::vector<vulkan::Framebuffer> Application::createSwapImageFramebuffers(const vulkan::RenderPass& render_pass) {
        // the visible attachment will be attachment 0
        
        std::vector<vulkan::Framebuffer> swap_framebuffers(getSwapImages().size());
        for(int i = 0; i < getSwapImages().size(); i++) {
            swap_framebuffers.at(i).setAttachment(0, getSwapImages().at(i).getImageView());
            swap_framebuffers.at(i).init(getDevice().getDevice(), render_pass, getSwapChain().getExtent());
        }

        return swap_framebuffers;
    } 

    void Application::recreateSwapImageFramebuffers(std::vector<vulkan::Framebuffer>& framebuffers, const vulkan::RenderPass& render_pass)  {
        // the visible attachment will be attachment 0

        for(int i = 0; i < getSwapImages().size(); i++) {
            framebuffers.at(i).cleanUp();
            framebuffers.at(i).setAttachment(0, getSwapImages().at(i).getImageView());
            framebuffers.at(i).init(getDevice().getDevice(), render_pass, getSwapChain().getExtent());
        }

    } 

    void Application::destroySwapImageFramebuffers(std::vector<vulkan::Framebuffer>& framebuffers) {

        for(vulkan::Framebuffer& f : framebuffers) 
            f.cleanUp();

    }

    graphics::Window& Application::getWindow() {

        return _window;
    }

    vulkan::LogicalDevice& Application::getDevice() {

        return _device;
    }

    vulkan::SwapChain& Application::getSwapChain() {

        return _swap_chain;
    }

    std::vector<vulkan::ImageView>& Application::getSwapImages() {

        return _swap_images;
    }

} // undicht