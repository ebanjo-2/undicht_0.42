#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "vector"
#include "vulkan/vulkan.h"
#include "render_pass.h"

namespace undicht {

    namespace vulkan {

        class Framebuffer {
            /** "Render passes operate in conjunction with framebuffers. 
             * Framebuffers represent a collection of specific memory attachments that a render pass instance uses."
             * (https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFramebuffer.html)*/
        protected:

            VkDevice _device_handle;

            VkFramebuffer _framebuffer;
            std::vector<VkImageView> _attachments;

            VkExtent2D _extent;

        public:

            /** attachments should be added before the framebuffer is initialized
             * the attachment type (color, depth/stencil attachment) is specified by the render_pass*/
            void setAttachment(int id, const VkImageView& image_view);

            /** @brief initializes the framebuffer
             * to change the extent, first resize all the attachments,
             * then call cleanUp(), reattach the image views and call init() once more */
            void init(const VkDevice& device, const RenderPass& render_pass, VkExtent2D extent);
            void cleanUp();

            const VkFramebuffer& getFramebuffer() const;
            const VkExtent2D& getExtent() const;

        protected:
            // creating framebuffer related structs

            VkFramebufferCreateInfo static createFramebufferCreateInfo(VkRenderPass render_pass, const std::vector<VkImageView>& attachments, VkExtent2D extent);

        };

    } // vulkan

} // undicht

#endif // FRAMEBUFFER_H