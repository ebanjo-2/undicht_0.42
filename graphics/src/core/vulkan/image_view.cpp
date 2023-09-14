#include "image_view.h"
#include "vk_debug.h"

#include "types.h"
#include "formats.h"

namespace undicht {

    namespace vulkan {

        void ImageView::init(const VkDevice& device, const VkImage& image, const VkFormat& format, uint32_t mip_levels, uint32_t layers, const VkImageViewType& view_type) {
            
            _device_handle = device;
            _image_handle = image;

            // storing the attributes
            _mip_levels = mip_levels;
            _layer_count = layers;
            _view_type = view_type;
            _format = format;

            // creating the image view
            VkImageViewCreateInfo info = createImageViewCreateInfo(image, mip_levels, layers, view_type, format, chooseImageAspectFlags(format));
            VK_ASSERT(vkCreateImageView(_device_handle, &info, {}, &_image_view));

        }

        void ImageView::cleanUp() {

            if(_image_view == VK_NULL_HANDLE) return;

            vkDestroyImageView(_device_handle, _image_view, {});
        }

        const VkImage& ImageView::getImage() const {

            return _image_handle;
        }

        const VkImageView& ImageView::getImageView() const {

            return _image_view;
        }

        const VkFormat& ImageView::getFormat() const {

            return _format;
        }

        //////////////////////////////////////// creating image view related structs //////////////////////////////////////

        VkImageViewCreateInfo ImageView::createImageViewCreateInfo(const VkImage& image, uint32_t mip_levels, uint32_t layer_count, const VkImageViewType& view_type, const VkFormat& format, VkImageAspectFlags flags) {
            
            VkImageViewCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.image = image;
            info.viewType = view_type;
            info.format = format;
            info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.subresourceRange.aspectMask = flags;
            info.subresourceRange.baseMipLevel = 0;
            info.subresourceRange.levelCount = mip_levels;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount = layer_count;
        
            return info;
        }

        VkImageAspectFlags ImageView::chooseImageAspectFlags(const VkFormat& format) {

            FixedType translated_type = translate(format);

            if(translated_type.m_type == Type::DEPTH_BUFFER)
                return VK_IMAGE_ASPECT_DEPTH_BIT;

            if(translated_type.m_type == Type::DEPTH_STENCIL_BUFFER)
                return VK_IMAGE_ASPECT_DEPTH_BIT;

            return VK_IMAGE_ASPECT_COLOR_BIT;
        }

    } // vulkan

} // undicht