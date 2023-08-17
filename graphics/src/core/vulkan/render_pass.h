#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include "cstdint"
#include "vector"
#include "vulkan/vulkan.h"

namespace undicht {

    namespace vulkan {

        class RenderPass {
            /** "A render pass object represents a collection of attachments, 
             * subpasses, and dependencies between the subpasses, 
             * and describes how the attachments are used over the course of the subpasses."
             * (https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkRenderPass.html)
            */
        protected:
            // a struct to store all the data needed by a suppass (which isnt its own vulkan object )
            struct SubPass {
                std::vector<uint32_t> _input_attachments;
                std::vector<VkImageLayout> _input_layouts;
                std::vector<uint32_t> _output_attachments;
                std::vector<VkImageLayout> _output_layouts;
                VkPipelineBindPoint _bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
            };

        protected:

            VkDevice _device_handle;

            VkRenderPass _render_pass = VK_NULL_HANDLE;
            std::vector<VkAttachmentDescription> _attachment_descriptions; // all attachments used by the render-pass
            
            std::vector<SubPass> _sub_passes;
            std::vector<VkSubpassDependency> _sub_pass_dependencies;

        public:

            /** adds an attachment (i.e. an image that stores color information) to the renderpass
             * @param initial_layout the layout the attachment will be in when starting the renderpass
             *  if clear_before_rendering is true, this can be VK_IMAGE_LAYOUT_UNDEFINED
             *  otherwise, if its a color attachment, use VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
             * @param final_layout the layout the attachment will be transitioned to at the end of the renderpass.
             *  if you want to present the attachment on a swap chain, use VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
             *  if you want to use the attachment as input for another render pass, you can use VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL 
             * @return the index with which the attachment can be accessed (counting up from 0) */
            int addAttachment(VkFormat format, VkImageLayout initial_layout, VkImageLayout final_layout, uint32_t samples = 1, bool clear_before_rendering = true, bool store_result = true);
            
            /** @brief adds a subpass to the renderpass
             * @param output_attachments declare which attachments of the render pass will be used as outputs of the subpass
             * @param output_layouts declare into which layout the attachments will be transitioned before being used as outputs.
             *  this depends on what kind of attachment is used:
             *  Color Attachments: should be VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
             *  Color Attachments: should be VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
             * @param input_attachments declare which attachments of the render pass will be used as inputs of the subpass
             * @param input_layouts declare in which layout the input attachments will be when accessing them
             * @return an id with which the subpass can be adressed in other functions */
            int addSubPass(const std::vector<uint32_t>& output_attachments, const std::vector<VkImageLayout>& output_layouts, const std::vector<uint32_t>& input_attachments = {}, const std::vector<VkImageLayout>& input_layouts = {}, VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);

            /** thanks for the explanation: https://www.reddit.com/r/vulkan/comments/s80reu/subpass_dependencies_what_are_those_and_why_do_i/
             * @brief tells the gpu to wait executing a subpass until another has finished (only needed if more than one subpass is used during rendering)
             * @param src_subpass the subpass that should be waited on
             * @param dst_subpass the subpass that should wait
             * @param src_stage the stage that has to finish in the src_subpass
             * @param dst_stage the stage the dst_subpass should wait at
             * @param src_access memory access types used by src_subpass
             * @param dst_access memory access types used by dst_subpass */
            void addSubPassDependency(uint32_t src_subpass, uint32_t dst_subpass, const VkPipelineStageFlags& src_stage, const VkPipelineStageFlags& dst_stage, const VkAccessFlags& src_access, const VkAccessFlags& dst_access);

            /** @brief once all output attachments and sub-passes are declared, call init 
             * if you want to change something about the renderpass, you have to first call cleanUp(), then init() again */
            void init(const VkDevice& device);
            void cleanUp();

            const VkRenderPass& getRenderPass() const;
            int getAttachmentCount() const;

        protected:
            // creating render-pass related structs

            VkAttachmentDescription static createAttachmentDescription(VkFormat format, VkImageLayout initial_layout, VkImageLayout final_layout, uint32_t samples, bool clear_before_rendering, bool store_result);
            VkAttachmentReference static createAttachmentReference(int attachment, VkImageLayout layout);
            VkSubpassDescription static createSubPassDescription(const std::vector<VkAttachmentReference>& color_attachments, const VkAttachmentReference& depth_attachment, const std::vector<VkAttachmentReference>& input_attachments, VkPipelineBindPoint bind_point);
            VkSubpassDependency static createSubPassDependency(uint32_t src_sub_pass, uint32_t dst_sub_pass, const VkPipelineStageFlags& src_stage, const VkPipelineStageFlags& dst_stage, const VkAccessFlags& src_access, const VkAccessFlags& dst_access);
            VkRenderPassCreateInfo static createRenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachments, const std::vector<VkSubpassDescription>& subpasses, const std::vector<VkSubpassDependency>& dependencies);

        };

    } // vulkan

} // undicht

#endif // RENDER_PASS_H