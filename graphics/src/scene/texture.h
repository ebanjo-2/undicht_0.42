#ifndef TEXTURE_H
#define TEXTURE_H

#include "vulkan/vulkan.h"
#include "vulkan_memory_allocator.h"

#include "core/vulkan/image.h"
#include "core/vulkan/image_view.h"
#include "core/vulkan/logical_device.h"

#include "types.h"

namespace undicht {

    class Texture {

      public:

        enum Type {
            DIFFUSE,
            SPECULAR,
            NORMAL,
            OTHER
        };

      protected:

        vulkan::LogicalDevice _device_handle;
        vma::VulkanMemoryAllocator _allocator_handle;

        // vulkan objects
        vulkan::Image _image;
        vulkan::ImageView _image_view;

        // attributes
        VkExtent3D _extent;
        uint32_t _nr_channels;
        FixedType _format;
        Type _type;


      public:

        void init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, Type type);
        void cleanUp();

        // assuming that each channel is one byte
        void setData(const void* data, uint32_t width, uint32_t height, uint32_t nr_channels);

        uint32_t getWidth() const;
        uint32_t getHeight() const;
        uint32_t getNrChannels() const;
        Type getType() const;

        const vulkan::Image& getImage() const;
        const vulkan::ImageView& getImageView() const;

      protected:
        // non public Texture functions

        void uploadToImage(vulkan::Image& dst, const void* data, uint32_t byte_size, VkExtent3D data_image_extent, VkOffset3D offset_in_image);

    };

} // undicht

#endif // TEXTURE_H