#ifndef IMAGE_H
#define IMAGE_H

#include "cstdint"
#include "vulkan/vulkan.h"
#include "vulkan_memory_allocator.h"

namespace undicht {

    namespace vulkan {

        class Image {
          
          protected:
            
			VmaAllocator _allocator_handle;

            VkImage _image;
            VkFormat _format;
            VkImageUsageFlags _usage;
            VkExtent3D _extent;
            uint32_t _layers;
            uint32_t _mip_levels;

          public:
            
            /** initializes the vulkan texture and allocates memory for it 
             * @param format you can use undicht::FixedType as a guide to choose a format
             * @param usage for color images use smth similar to VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
             * for depth buffers use smth similar to VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
             * @param layers create array images 
             * @param samples specify a higher number than one if you want to use the texture as an output of a multisampled rendering operation
             * @param flags used to create cube maps (use VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) */
            void init(vma::VulkanMemoryAllocator& allocator, VkFormat format, VkImageUsageFlags usage, VkExtent3D extent, uint32_t layers = 1, uint32_t mip_levels = 1, uint32_t samples = 1, VkImageCreateFlags flags = {});
            void cleanUp();

            const VkImage& getImage() const;

          protected:
            // creating image related structs

            VkImageCreateInfo static createImageCreateInfo(VkExtent3D extent, uint32_t layers, uint32_t mip_levels, uint32_t samples, VkFormat format, VkImageUsageFlags usage, VkImageCreateFlags flags);

          public:

            

        };

    } // vulkan

} // undicht

#endif // IMAGE_H