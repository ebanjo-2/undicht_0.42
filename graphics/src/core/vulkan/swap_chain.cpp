#include "swap_chain.h"

#include "cassert"

#include "debug.h"
#include "vk_debug.h"

namespace undicht {

    namespace vulkan {

        void SwapChain::init(const LogicalDevice& device, const VkSurfaceKHR& surface, VkPresentModeKHR preferred) {

            _device_handle = device.getDevice();
            _physical_device_handle = device.getPhysicalDevice();
            _surface_handle = surface;

            // getting the unique queue families that are going to access the swap chain
            _graphics_queue_family = device.getGraphicsQueueFamily();
            _present_queue_family = device.getPresentQueueFamily();

            // choosing the features of the swap chain
            _extent = chooseExtent(_physical_device_handle, surface);
            _surface_format = chooseSurfaceFormat(_physical_device_handle, surface);
            _present_mode = choosePresentMode(_physical_device_handle, surface, preferred);
            _swap_image_count = chooseSwapImageCount(_physical_device_handle, surface);
            _surface_transform_flags = chooseSurfaceTransformFlags(_physical_device_handle, surface);

            // queue families that are going to use the swap chain
            std::vector<uint32_t> queue_families = {_graphics_queue_family};
            if(_graphics_queue_family != _present_queue_family) queue_families.push_back(_present_queue_family);

            // creating the swap chain
            VkSwapchainCreateInfoKHR info = createSwapchainCreateInfo(surface, _swap_image_count,_surface_format, _present_mode, _extent, queue_families, _surface_transform_flags);
            VK_ASSERT(vkCreateSwapchainKHR(_device_handle, &info, {}, &_swap_chain));
            
        }

        void SwapChain::cleanUp() {

            // waiting for processes that are using the swap chain to finish
            VK_ASSERT(vkDeviceWaitIdle(_device_handle));

            // destroying the swap chain
            vkDestroySwapchainKHR(_device_handle, _swap_chain, {});
        }

        std::vector<ImageView> SwapChain::retrieveSwapImages() {

            std::vector<VkImage> vk_images(_swap_image_count);
            VK_ASSERT(vkGetSwapchainImagesKHR(_device_handle, _swap_chain, &_swap_image_count, vk_images.data()));

            // storing the swap images
            std::vector<ImageView> swap_images(_swap_image_count);
            for(int i = 0; i < vk_images.size(); i++)
                swap_images[i].init(_device_handle, vk_images[i], _surface_format.format);

            return swap_images;
        }

        void SwapChain::freeSwapImages(std::vector<ImageView>& swap_images) {

            for(ImageView& image : swap_images)
                image.cleanUp();

        }

        void SwapChain::recreate(const LogicalDevice& device, const VkSurfaceKHR &surface, VkPresentModeKHR preferred) {
            // should be called when the surface was changed (i.e. the window it belongs to was resized)
            // you also need to free any old swap images and retrieve them again

            cleanUp();
            init(device, surface, preferred);

        }

        uint32_t SwapChain::acquireNextSwapImage(VkSemaphore signal_sem, VkFence signal_fen) {
            /// @brief request image from the swapchain
            /// @param signal_sem (optional) gets signaled once the image is ready
            /// @param signal_fen (optional) gets signaled once the image is ready
            /// @return the id of the newly acquired image, -1 if no image could be acquired (swapchain needs to be recreated)

            uint32_t next_image;
            VkResult result = vkAcquireNextImageKHR(_device_handle, _swap_chain, UINT64_MAX, signal_sem, signal_fen, &next_image);

            if(result != VK_SUCCESS) {
                UND_WARNING << "swapchain is out of date \n";
                return -1;
            }

            return next_image;
        }

        /////////////////////////////////////// "getters" for vulkan objects ///////////////////////////////////////

        const VkSwapchainKHR& SwapChain::getSwapchain() const {

            return _swap_chain;
        }

        const VkFormat& SwapChain::getSwapImageFormat() const {

            return _surface_format.format;
        }

        const VkExtent2D& SwapChain::getExtent() const {

            return _extent;
        }

        int SwapChain::getSwapImageCount() const {

            return _swap_image_count;
        }

        ////////////////////////////////// choosing features for the swap chain //////////////////////////////////

        VkExtent2D SwapChain::chooseExtent(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface) {

            VkSurfaceCapabilitiesKHR capabilities;
            VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities));

            return capabilities.currentExtent;
        }

        VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface) {

            // getting the formats supported by the physical device and the surface
            unsigned format_count;
            VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr));
            std::vector<VkSurfaceFormatKHR> available_formats(format_count);
            VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, available_formats.data()));

            // looking for the preferred format
            VkSurfaceFormatKHR BGRA_srgb {VK_FORMAT_B8G8R8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR};

            for(VkSurfaceFormatKHR& format : available_formats)
                if(format.format == BGRA_srgb.format && format.colorSpace == BGRA_srgb.colorSpace)
                    return BGRA_srgb;

            // returning an undefined alternative format (may result in a crash or weird resulting images)
            UND_WARNING << "preferred swap chain format VK_FORMAT_B8G8R8A8_SRGB is not supported\n";

            // if there are no available formats the application is doomed anyway
            assert(available_formats.size());

            return available_formats.front();
        }

        VkPresentModeKHR SwapChain::choosePresentMode(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface, VkPresentModeKHR preferred) {

            // getting the present modes supported by the physical device and the surface
            unsigned present_mode_count;
            VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr));
            std::vector<VkPresentModeKHR> present_modes(present_mode_count);
            VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data()));

            // looking for the preferred present mode

            for(VkPresentModeKHR& mode : present_modes)
                if(mode == preferred)
                    return preferred;

            // returning an alternative present mode (vsync mode is guaranteed to exist)
            UND_WARNING << "Using alternative present mode : vsync\n";
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        uint32_t SwapChain::chooseSwapImageCount(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface) {

            // getting the surface capabilities
            VkSurfaceCapabilitiesKHR capabilities;
            VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities));

            // choosing a swap image count
            uint32_t count = 2;
            count = std::max(count, capabilities.minImageCount);

            if(capabilities.maxImageCount)
                count = std::min(count, capabilities.maxImageCount);

            return count;
        }

        VkSurfaceTransformFlagBitsKHR SwapChain::chooseSurfaceTransformFlags(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface) {

            VkSurfaceCapabilitiesKHR capabilities;
            VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities));

            return capabilities.currentTransform;
        }

        //////////////////////////////////  creating swap chain related structs //////////////////////////////////

        VkSwapchainCreateInfoKHR SwapChain::createSwapchainCreateInfo(const VkSurfaceKHR& surface,uint32_t image_count, const VkSurfaceFormatKHR& format, const VkPresentModeKHR& present_mode, const VkExtent2D& extent, const std::vector<uint32_t>& queue_families, const VkSurfaceTransformFlagBitsKHR& transform) {

            VkSwapchainCreateInfoKHR info{};
            info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            info.surface = surface;
            info.minImageCount = image_count;
            info.imageFormat = format.format;
            info.imageColorSpace = format.colorSpace;
            info.imageExtent = extent;
            info.imageArrayLayers = 1;
            info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            if(queue_families.size() == 1)
                info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            else
                info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;

            info.pQueueFamilyIndices = queue_families.data();
            info.queueFamilyIndexCount = queue_families.size();
            info.preTransform = transform;
            info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            info.presentMode = present_mode;
            info.clipped = VK_TRUE;

            return info;
        }

    } // vulkan

} // undicht