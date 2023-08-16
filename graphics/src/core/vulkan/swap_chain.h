#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H

#include "vulkan/vulkan.h"
#include "logical_device.h"
#include "image_view.h"

namespace undicht {

    namespace vulkan {

        class SwapChain {
            /* "A swapchain object (a.k.a. swapchain) provides the ability to present rendering results to a surface" 
            * "A swapchain is an abstraction for an array of presentable images that are associated with a surface. 
            * The presentable images are represented by VkImage objects created by the platform"
            * Source: https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSwapchainKHR.html */
          protected:

            // vulkan handles
            VkDevice _device_handle;
            VkPhysicalDevice _physical_device_handle;
            VkSurfaceKHR _surface_handle;

            uint32_t _graphics_queue_family;
            uint32_t _present_queue_family;

            // the swapchain object
            VkSwapchainKHR _swap_chain = VK_NULL_HANDLE;

            // attributes of the swap chain
            VkSurfaceFormatKHR _surface_format;
            VkPresentModeKHR _present_mode;
            VkExtent2D _extent;
            VkSurfaceTransformFlagBitsKHR _surface_transform_flags;

            uint32_t _swap_image_count = 0;

          public:

            void init(const LogicalDevice& device, const VkSurfaceKHR& surface, VkPresentModeKHR preferred = VK_PRESENT_MODE_IMMEDIATE_KHR);
            void cleanUp();

            std::vector<ImageView> retrieveSwapImages();
            void freeSwapImages(std::vector<ImageView>& swap_images);

            // if the surfaces properties have changed (most likely its size) the swap chain needs to be recreated
            // you also need to free any old swap images and retrieve them again
            void recreate(const LogicalDevice& device, const VkSurfaceKHR& surface, VkPresentModeKHR preferred = VK_PRESENT_MODE_IMMEDIATE_KHR);

            /// @brief accquire an image from the swapchain which can be modified / rendered to 
            ///        until it is queued for presentation via vkQueuePresentKHR (LogicalDevice)
            /// @param signal_sem (optional) gets signaled once the image is ready
            /// @param signal_fen (optional) gets signaled once the image is ready
            /// @return the id of the newly acquired image, -1 if no image could be acquired (swapchain needs to be recreated)
            uint32_t acquireNextSwapImage(VkSemaphore signal_sem = VK_NULL_HANDLE, VkFence signal_fen = VK_NULL_HANDLE);

            // "getters" for vulkan objects
            const VkSwapchainKHR& getSwapchain() const;
            const VkFormat& getSwapImageFormat() const;
            const VkExtent2D& getExtent() const;
            const VkImageView& getSwapImageView(int id) const;
            int getSwapImageCount() const;

          protected:
            // choosing features for the swap chain
            
            VkExtent2D static chooseExtent(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);
            VkSurfaceFormatKHR static chooseSurfaceFormat(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);
            VkPresentModeKHR static choosePresentMode(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface, VkPresentModeKHR preferred = VK_PRESENT_MODE_IMMEDIATE_KHR);
            uint32_t static chooseSwapImageCount(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);
            VkSurfaceTransformFlagBitsKHR static chooseSurfaceTransformFlags(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);
            
        protected:
            //  creating swap chain related structs

            VkSwapchainCreateInfoKHR static createSwapchainCreateInfo(const VkSurfaceKHR& surface, uint32_t image_count, const VkSurfaceFormatKHR& format, const VkPresentModeKHR& present_mode, const VkExtent2D& extent, const std::vector<uint32_t>& queue_families, const VkSurfaceTransformFlagBitsKHR& transform);
        };

    }

} // undicht

#endif // SWAP_CHAIN_H