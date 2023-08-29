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

            VkImageCreateInfo image_info = createImageCreateInfo(extent, layers, mip_levels, samples, format, usage, flags);

            VmaAllocationCreateInfo alloc_info = {};
            alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
            alloc_info.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
            alloc_info.priority = 1.0f;

            vmaCreateImage(allocator.getVmaAllocator(), &image_info, &alloc_info, &_image, &_allocation, nullptr);
            vmaGetAllocationMemoryProperties(allocator.getVmaAllocator(), _allocation, &_mem_prop_flags);

        }

        void Image::cleanUp() {

            vmaDestroyImage(_allocator_handle, _image, _allocation);
        }

        const VkImage& Image::getImage() const {

            return _image;
        }

        const VkFormat& Image::getFormat() const {
            
            return _format;
        }

        const VkExtent3D& Image::getExtent() const {
            
            return _extent;
        }

        const uint32_t& Image::getLayers() const {
            
            return _layers;
        }

        const uint32_t& Image::getMipLevels() const {

            return _mip_levels;
        }

        //////////////////////////// creating image related structs ////////////////////////////

        VkImageSubresourceRange Image::createImageSubresourceRange(VkImageAspectFlags flags, uint32_t base_mip_level, uint32_t num_mip_levels, uint32_t base_layer, uint32_t layer_count) {

            VkImageSubresourceRange range{};
		    range.aspectMask = flags;
		    range.baseMipLevel = base_mip_level;
		    range.levelCount = num_mip_levels;
		    range.baseArrayLayer = base_layer;
		    range.layerCount = layer_count;

            return range;
        }
        
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

        VkImageMemoryBarrier Image::createImageMemoryBarrier(VkImage image, VkImageSubresourceRange range, VkImageLayout old_layout, VkImageLayout new_layout, VkAccessFlags src_access, VkAccessFlags dst_access) {
            // used to transition an image from one layout the another one

            VkImageMemoryBarrier barrier{};
		    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		    barrier.oldLayout = old_layout;
		    barrier.newLayout = new_layout;
		    barrier.image = image;
		    barrier.subresourceRange = range;
		    barrier.srcAccessMask = src_access;
		    barrier.dstAccessMask = dst_access;

            return barrier;
        }

        VkBufferImageCopy Image::createBufferImageCopy(VkExtent3D image_extent, VkOffset3D image_offset, VkImageAspectFlags flags, uint32_t layer, uint32_t mip_level, uint32_t src_offset) {

            VkBufferImageCopy region{};
            region.bufferOffset = src_offset; // layout of the data in the buffer
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask = flags;
            region.imageSubresource.mipLevel = mip_level;
            region.imageSubresource.baseArrayLayer = layer;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = image_offset;
            region.imageExtent = image_extent;

            return region;
        }

        VkImageBlit Image::createImageBlit(int src_width, int src_height, uint32_t src_mip_level, uint32_t dst_mip_level) {

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = {src_width, src_height, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = src_mip_level;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = {src_width > 1 ? src_width / 2 : 1, src_height > 1 ? src_height / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = dst_mip_level;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            return blit;
        }

    } // vulkan 

} // undicht