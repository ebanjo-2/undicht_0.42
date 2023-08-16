#ifndef IMAGE_VIEW_H
#define IMAGE_VIEW_H

#include "vulkan/vulkan.h"

namespace undicht {

    namespace vulkan {

        class ImageView {

          protected:

            VkDevice _device_handle = VK_NULL_HANDLE;
            VkImage _image_handle = VK_NULL_HANDLE;

            VkImageView _image_view = VK_NULL_HANDLE;

            // attributes of the image view
            uint32_t _mip_levels;
            uint32_t _layer_count = 1;
            VkImageViewType _view_type;
            VkFormat _format;

          public:

            void init(const VkDevice& device, const VkImage& image, const VkFormat& format = VK_FORMAT_R8G8B8A8_SRGB, uint32_t mip_levels = 1, uint32_t layers = 1, const VkImageViewType& view_type = VK_IMAGE_VIEW_TYPE_2D);
            void cleanUp();
            
            const VkImage& getImage() const;
            const VkImageView& getImageView() const;
            const VkFormat& getFormat() const;

          protected:
            // creating image view related structs

            VkImageViewCreateInfo static createImageViewCreateInfo(const VkImage& image, uint32_t mip_levels, uint32_t layer_count, const VkImageViewType& view_type, const VkFormat& format, VkImageAspectFlags flags);
            VkImageAspectFlags static chooseImageAspectFlags(const VkFormat& format);

        };

    } // vulkan

} // undicht

#endif // IMAGE_VIEW