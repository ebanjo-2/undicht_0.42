#include "render_pass.h"

#include "vk_debug.h"

namespace undicht {

    namespace vulkan {

        int RenderPass::addAttachment(VkFormat format, VkImageLayout initial_layout, VkImageLayout final_layout, uint32_t samples, bool clear_before_rendering, bool store_result) {
            /** adds an attachment (i.e. an image that stores color information) to the renderpass
             * @param final_layout the layout the attachment will be transitioned to at the end of the renderpass.
             *  if you want to present the attachment on a swap chain, use VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
             *  if you want to use the attachment as input for another render pass, you can use VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL 
             * @return the index with which the attachment can be accessed (counting up from 0) */

            _attachment_descriptions.push_back(createAttachmentDescription(format, initial_layout, final_layout, samples, clear_before_rendering, store_result));

            return _attachment_descriptions.size() - 1;
        }


        int RenderPass::addSubPass(const std::vector<uint32_t>& output_attachments, const std::vector<VkImageLayout>& output_layouts, const std::vector<uint32_t>& input_attachments, const std::vector<VkImageLayout>& input_layouts, VkPipelineBindPoint bind_point) {
            /** @brief adds a subpass to the renderpass
             * @param output_attachments declare which attachments of the render pass will be used as outputs of the subpass
             * @param output_layouts declare into which layout the attachments will be transitioned before being used as outputs.
             *  this depends on what kind of attachment is used:
             *  Color Attachments: should be VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
             *  Color Attachments: should be VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
             * @param input_attachments declare which attachments of the render pass will be used as inputs of the subpass
             * @param input_layouts declare in which layout the input attachments will be when accessing them
             * @return an id with which the subpass can be adressed in other functions */

            SubPass new_sub_pass{};
            new_sub_pass._bind_point = bind_point;
            new_sub_pass._input_attachments = input_attachments;
            new_sub_pass._input_layouts = input_layouts;
            new_sub_pass._output_attachments = output_attachments;
            new_sub_pass._output_layouts = output_layouts;

            _sub_passes.push_back(new_sub_pass);

            return _sub_passes.size() - 1;
        }

        void RenderPass::addSubPassDependency(uint32_t src_subpass, uint32_t dst_subpass, const VkPipelineStageFlags& src_stage, const VkPipelineStageFlags& dst_stage, const VkAccessFlags& src_access, const VkAccessFlags& dst_access) {
            /** thanks for the explanation: https://www.reddit.com/r/vulkan/comments/s80reu/subpass_dependencies_what_are_those_and_why_do_i/
             * @brief tells the gpu to wait executing a subpass until another has finished (only needed if more than one subpass is used during rendering)
             * @param src_subpass the subpass that should be waited on
             * @param dst_subpass the subpass that should wait
             * @param src_stage the stage that has to finish in the src_subpass
             * @param dst_stage the stage the dst_subpass should wait at
             * @param src_access memory access types used by src_subpass
             * @param dst_access memory access types used by dst_subpass */

            _sub_pass_dependencies.push_back(createSubPassDependency(src_subpass, dst_subpass, src_stage, dst_stage, src_access, dst_access));
        }

        void RenderPass::init(const VkDevice& device) {
            /** @brief once all output attachments and sub-passes are declared, call init 
             * if you want to change something about the renderpass, you have to first call cleanUp(), then init() again */

            _device_handle = device;

            // creating subpass descriptions
            std::vector<VkSubpassDescription> sub_pass_descriptions(_sub_passes.size());
            std::vector<std::vector<VkAttachmentReference>> input_references(_sub_passes.size());
            std::vector<std::vector<VkAttachmentReference>> color_references(_sub_passes.size());
            std::vector<VkAttachmentReference> depth_reference(_sub_passes.size(), {VK_ATTACHMENT_UNUSED});

            for(int sub_index = 0; sub_index < _sub_passes.size(); sub_index++) {

                const SubPass& sub = _sub_passes[sub_index];

                // references for attachments used as inputs 
                for(int j = 0; j < sub._input_attachments.size(); j++)
                    input_references.at(sub_index).push_back(createAttachmentReference(sub._input_attachments[j], sub._input_layouts[j]));

                // references for attachments used as outputs 
                for(int j = 0; j < sub._output_attachments.size(); j++) {

                    if(sub._output_layouts[j] == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                        color_references.at(sub_index).push_back(createAttachmentReference(sub._output_attachments[j], sub._output_layouts[j]));
                    else if(sub._output_layouts[j] == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                        depth_reference.at(sub_index) = createAttachmentReference(sub._output_attachments[j], sub._output_layouts[j]);
                
                }

            }

            for(int sub_index = 0; sub_index < _sub_passes.size(); sub_index++)
                sub_pass_descriptions.at(sub_index) = createSubPassDescription(color_references.at(sub_index), depth_reference.at(sub_index), input_references.at(sub_index), _sub_passes.at(sub_index)._bind_point);

            // creating the render pass
            VkRenderPassCreateInfo info = createRenderPassCreateInfo(_attachment_descriptions, sub_pass_descriptions, _sub_pass_dependencies);
            VK_ASSERT(vkCreateRenderPass(device, &info, {}, &_render_pass));

        }

        void RenderPass::cleanUp() {

            vkDestroyRenderPass(_device_handle, _render_pass, {});
        }

        const VkRenderPass& RenderPass::getRenderPass() const {

            return _render_pass;
        }

        int RenderPass::getAttachmentCount() const {

            return _attachment_descriptions.size();
        }

        ////////////////////////////// creating render-pass related structs //////////////////////////////

        VkAttachmentDescription RenderPass::createAttachmentDescription(VkFormat format, VkImageLayout initial_layout, VkImageLayout final_layout, uint32_t samples, bool clear_before_rendering, bool store_result) {

            VkAttachmentDescription attachment{};
            attachment.format = format;
            attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            attachment.initialLayout = initial_layout;
            attachment.finalLayout = final_layout;

            if(clear_before_rendering) 
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            else 
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            
            if(store_result)
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            else
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_NONE_KHR;

            // no stencil testing for now
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

            return attachment;
        }

        VkAttachmentReference RenderPass::createAttachmentReference(int attachment, VkImageLayout layout) {

            VkAttachmentReference reference{};
            reference.attachment = attachment;
            reference.layout = layout;

            return reference;
        }

        VkSubpassDescription RenderPass::createSubPassDescription(const std::vector<VkAttachmentReference>& color_attachments, const VkAttachmentReference& depth_attachment, const std::vector<VkAttachmentReference>& input_attachments, VkPipelineBindPoint bind_point) {

            // creating the sub-pass description
            VkSubpassDescription description{};
            description.pipelineBindPoint = bind_point;
            description.colorAttachmentCount = color_attachments.size();
            description.pColorAttachments = color_attachments.data();
            description.pDepthStencilAttachment = &depth_attachment;
            description.inputAttachmentCount = input_attachments.size();
            description.pInputAttachments = input_attachments.data();

            return description;
        }

        VkSubpassDependency RenderPass::createSubPassDependency(uint32_t src_sub_pass, uint32_t dst_sub_pass, const VkPipelineStageFlags& src_stage, const VkPipelineStageFlags& dst_stage, const VkAccessFlags& src_access, const VkAccessFlags& dst_access) {

            VkSubpassDependency dependency{};

		    dependency.srcSubpass = src_sub_pass;
		    dependency.dstSubpass = dst_sub_pass;
		    dependency.srcStageMask = src_stage;
		    dependency.dstStageMask = dst_stage;
		    dependency.srcAccessMask = src_access;
		    dependency.dstAccessMask = dst_access;
		    dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            return dependency;
        }

        VkRenderPassCreateInfo RenderPass::createRenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachments, const std::vector<VkSubpassDescription>& subpasses, const std::vector<VkSubpassDependency>& dependencies) {

            VkRenderPassCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            info.attachmentCount = attachments.size();
            info.pAttachments = attachments.data();
            info.subpassCount = subpasses.size();
            info.pSubpasses = subpasses.data();
            info.dependencyCount = dependencies.size();
            info.pDependencies = dependencies.data();

            return info;
        }

    } // vulkan

} // undicht