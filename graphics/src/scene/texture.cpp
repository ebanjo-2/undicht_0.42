#include "texture.h"
#include "core/vulkan/buffer.h"
#include "core/vulkan/command_buffer.h"
#include "core/vulkan/fence.h"
#include "core/vulkan/formats.h"

namespace undicht {

    namespace graphics {

        using namespace vulkan;

        void Texture::init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, Type type) {

            _device_handle = device;
            _allocator_handle = allocator;
            _type = type;

        }

        void Texture::cleanUp() {

            _image.cleanUp();
            _image_view.cleanUp();

        }

        void Texture::setData(const void* data, uint32_t width, uint32_t height, uint32_t nr_channels, TransferBuffer& transfer_buffer) {
            // assuming that each channel is one byte

            _extent.width = width;
            _extent.height = height;
            _extent.depth = 1;
            _nr_channels = nr_channels;
            _format = FixedType(undicht::Type::COLOR_RGBA_SRGB, 1, nr_channels);
            _mip_levels = calcMipLevelCount(width, height);

            VkImageUsageFlags image_usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

            _image.init(_allocator_handle, translate(_format), _extent, image_usage, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, {}, 1, _mip_levels);
            _image_view.init(_device_handle.getDevice(), _image.getImage(), translate(_format), _mip_levels);

            // store data in image
            transfer_buffer.stageForTransfer(_image.getImage(), data, width * height * nr_channels, _extent);

        }

        void Texture::genMipMaps(vulkan::CommandBuffer& cmd) {
            // records the commands to generate the mip maps
            // source: https://vulkan-tutorial.com/Generating_Mipmaps
            
            // transition the mip level 0 to a layout that allows reading it
            VkImageSubresourceRange base_range = Image::createImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
            VkImageMemoryBarrier base_barrier = Image::createImageMemoryBarrier(_image.getImage(), base_range, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_NONE, VK_ACCESS_TRANSFER_READ_BIT);
            cmd.pipelineBarrier(base_barrier, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

            // transition all other mip levels to a layout that allows writing to them
            VkImageSubresourceRange mip_levels_range = Image::createImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 1, _mip_levels - 1, 0, 1);
            VkImageMemoryBarrier mip_levels_barrier = Image::createImageMemoryBarrier(_image.getImage(), mip_levels_range, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT);
            cmd.pipelineBarrier(mip_levels_barrier, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

            int32_t mip_width = _extent.width;
            int32_t mip_height = _extent.height;

            for (uint32_t i = 1; i < _mip_levels; i++) {
                // create mip maps for each mip level (starting at 1)

                // blit operation from the upper mip level to the lower mip level
                VkImageBlit blit = Image::createImageBlit(mip_width, mip_height, i - 1, i);
                cmd.blitImage(_image.getImage(), blit);

                // transition the  higher level to a layout that is readable by a shader
                VkImageSubresourceRange upper_level_range = Image::createImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, i - 1, 1, 0, 1);
                VkImageMemoryBarrier upper_barrier = Image::createImageMemoryBarrier(_image.getImage(), upper_level_range, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_READ_BIT);
                cmd.pipelineBarrier(upper_barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

                // transition the lower level to a layout that allows reading from it
                VkImageSubresourceRange lower_level_range = Image::createImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, i, 1, 0, 1);
                VkImageMemoryBarrier lower_barrier = Image::createImageMemoryBarrier(_image.getImage(), lower_level_range, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT);
                cmd.pipelineBarrier(lower_barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
 
                // calc size of next mip level
                if (mip_width > 1) mip_width /= 2;
                if (mip_height > 1) mip_height /= 2;

            }

            // transition the last mip level to a layout that allows reading in for a shader
            VkImageSubresourceRange lowest_level_range = Image::createImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels - 1, 1, 0, 1);
            VkImageMemoryBarrier lowest_level_barrier = Image::createImageMemoryBarrier(_image.getImage(), lowest_level_range, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_NONE, VK_ACCESS_SHADER_READ_BIT);
            cmd.pipelineBarrier(lowest_level_barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        }

        uint32_t Texture::getWidth() const {

            return _extent.width;
        }

        uint32_t Texture::getHeight() const {

            return _extent.height;
        }

        uint32_t Texture::getNrChannels() const {

            return _nr_channels;
        }

        Texture::Type Texture::getType() const {

            return _type;
        }

        const vulkan::Image& Texture::getImage() const {

            return _image;
        }

        const vulkan::ImageView& Texture::getImageView() const {

            return _image_view;
        }

        ///////////////////////////////// non public Texture functions /////////////////////////////////

        uint32_t Texture::calcMipLevelCount(uint32_t width, uint32_t height) {

            uint32_t max_dim = std::max(width, height);
            uint32_t mip_levels = 1;

            while(max_dim /= 2) mip_levels++;

            return mip_levels;
        }

    } // graphics

} // undicht