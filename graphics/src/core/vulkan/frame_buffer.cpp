#include "frame_buffer.h"

#include "cassert"
#include "debug.h"
#include "vk_debug.h"

namespace undicht {

    namespace vulkan {


        void Framebuffer::setAttachment(int id, const VkImageView& image_view) {
            /** attachments should be added before the framebuffer is initialized
            * the attachment type (color, depth/stencil attachment) is specified by the render_pass*/

            if(_attachments.size() <= id)
                _attachments.resize(id + 1, {});

            _attachments.at(id) = image_view;
        }

        void Framebuffer::init(const VkDevice& device, const RenderPass& render_pass, VkExtent2D extent) {
            /** @brief initializes the framebuffer
             * to change the extent, first resize all the attachments,
             * then call cleanUp(), reattach the image views and call init() once more */

            _device_handle = device;
            _extent = extent;

            // if this fails, you dont have the right amount of attachments attached to the framebuffer
            assert(render_pass.getAttachmentCount() == _attachments.size());

            VkFramebufferCreateInfo info = createFramebufferCreateInfo(render_pass.getRenderPass(), _attachments, extent);
            VK_ASSERT(vkCreateFramebuffer(device, &info, {}, &_framebuffer));

        }

        void Framebuffer::cleanUp() {

            if(_device_handle == VK_NULL_HANDLE)
                return;

            _attachments.clear();
            vkDestroyFramebuffer(_device_handle, _framebuffer, {});
            
        }

        const VkFramebuffer& Framebuffer::getFramebuffer() const {

            return _framebuffer;
        }

        const VkExtent2D& Framebuffer::getExtent() const {

            return _extent;
        }


        /////////////////////////////// creating framebuffer related structs ///////////////////////////////


        VkFramebufferCreateInfo Framebuffer::createFramebufferCreateInfo(VkRenderPass render_pass, const std::vector<VkImageView>& attachments, VkExtent2D extent) {

            VkFramebufferCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            info.pNext = nullptr;
            info.renderPass = render_pass;
            info.attachmentCount = attachments.size();
            info.pAttachments = attachments.data();
            info.width = extent.width;
            info.height = extent.height;
            info.layers = 1;

            return info;
        }


    } // vulkan

} // undicht