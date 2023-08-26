#include "image.h"

namespace undicht {

    namespace vulkan {

        void Image::init(vma::VulkanMemoryAllocator& allocator, VkFormat format, VkImageUsageFlags usage, VkExtent3D extent, uint32_t layers, uint32_t mip_levels, uint32_t samples, VkImageCreateFlags flags) {
            /** initializes the vulkan texture and allocates memory for it 
             * @param format you can use undicht::FixedType as a guide to choose a format
             * @param usage for color images use smth similar to VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
             * for depth buffers use smth similar to VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
             * @param layers create array images 
             * @param samples specify a higher number than one if you want to use the texture as an output of a multisampled rendering operation
             * @param flags used to create cube maps (use VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) */

            _allocator_handle = allocator.getVmaAllocator();
            _format = format;
            _extent = extent;
            _layers = layers;
            _mip_levels = mip_levels;

            VkImageCreateInfo info = createImageCreateInfo(extent, layers, mip_levels, samples, format, usage, flags);

        }

        void Image::cleanUp() {

        }

        //////////////////////////// creating image related structs ////////////////////////////

        VkImageCreateInfo Image::createImageCreateInfo(VkExtent3D extent, uint32_t layers, uint32_t mip_levels, uint32_t samples, VkFormat format, VkImageUsageFlags usage, VkImageCreateFlags flags) {

            VkImageCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            info.extent = extent;
            info.imageType = extent.depth == 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
            info.arrayLayers = layers;
            info.mipLevels = mip_levels;
            info.format = format;
            info.tiling = VK_IMAGE_TILING_OPTIMAL;
            info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            info.usage = usage;
            info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // used exclusively by the graphics queue
            info.samples = (VkSampleCountFlagBits)samples; // used for multisampling
            info.flags = flags;

            return info;
        }
    } // vulkan 

} // undicht