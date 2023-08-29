#include "texture.h"
#include "core/vulkan/buffer.h"
#include "core/vulkan/command_buffer.h"
#include "core/vulkan/fence.h"
#include "core/vulkan/formats.h"

namespace undicht {

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

    void Texture::setData(const void* data, uint32_t width, uint32_t height, uint32_t nr_channels) {
        // assuming that each channel is one byte

        _extent.width = width;
        _extent.height = height;
        _extent.depth = 1;
        _nr_channels = nr_channels;
        _format = FixedType(undicht::Type::COLOR_RGBA_SRGB, 1, nr_channels);

        // store data in image
        _image.init(_allocator_handle, translate(_format), VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, _extent);
        uploadToImage(_image, data, width * height * nr_channels, _extent, {0, 0, 0});

        // init image view
        _image_view.init(_device_handle.getDevice(), _image.getImage(), translate(_format));

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

    ///////////////////////////////////////// non public Texture functions /////////////////////////////////////////

    void Texture::uploadToImage(Image& dst, const void* data, uint32_t byte_size, VkExtent3D data_image_extent, VkOffset3D offset_in_image) {

        // init the staging buffer
        Buffer staging_buffer;
        VkBufferUsageFlags tmp_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        VmaAllocationCreateFlags tmp_vma_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        staging_buffer.init(_allocator_handle, {_device_handle.getGraphicsQueueFamily()}, byte_size, tmp_usage_flags, tmp_vma_flags);

        // store data in staging buffer
        staging_buffer.setData(byte_size, 0, data);

        // set up info structs
        VkBufferImageCopy copy_info = Image::createBufferImageCopy(dst.getExtent(), {0, 0, 0}, VK_IMAGE_ASPECT_COLOR_BIT);
        VkImageSubresourceRange range = Image::createImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
        VkImageMemoryBarrier first_barrier = Image::createImageMemoryBarrier(_image.getImage(), range, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT);   
        VkImageMemoryBarrier second_barrier = Image::createImageMemoryBarrier(_image.getImage(), range, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);

        // record command buffer
        CommandBuffer cmd;
        cmd.init(_device_handle.getDevice(), _device_handle.getGraphicsCmdPool());
        cmd.beginCommandBuffer(true);
        cmd.pipelineBarrier(first_barrier, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT); // transition to layout that allows transfering data to the texture
        cmd.copy(staging_buffer.getBuffer(), dst.getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copy_info); // copy data to image
        cmd.pipelineBarrier(second_barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT); // transition to layout that allows sampling the texture in a shader
        cmd.endCommandBuffer();

        // set up fence as a way to know when the copy operation has finished
        Fence copy_finished;
        copy_finished.init(_device_handle.getDevice(), false);

        // submit copy cmd + clean up
        _device_handle.submitOnGraphicsQueue(cmd.getCommandBuffer(), copy_finished.getFence());
        copy_finished.waitForProcessToFinish();
        copy_finished.cleanUp();
        cmd.cleanUp();
        staging_buffer.cleanUp();

    }

} // undicht