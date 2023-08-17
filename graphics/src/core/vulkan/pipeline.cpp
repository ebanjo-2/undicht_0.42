#include "pipeline.h"

#include "cassert"
#include "debug.h"
#include "vk_debug.h"
#include "formats.h"

namespace undicht {

    namespace vulkan {

        //////////////////////////////// configuration ///////////////////////////////

        void Pipeline::setViewport(const VkExtent2D& extent) {

            _viewport = createViewport(extent);
            _scissor = createScissor(extent);
            _viewport_state = createPipelineViewportStateCreateInfo(_viewport, _scissor);

        }

        void Pipeline::addShaderModule(const ShaderModule& module) {
            
            _shader_stages.push_back(createPipelineShaderStageCreateInfo(module.getShaderStageFlagBits(), module.getShaderModule()));
        }

        void Pipeline::addVertexBinding(uint32_t binding, uint32_t total_stride) {
            /** "A vertex binding describes at which rate to load data from memory throughout the vertices. 
             * It specifies the number of bytes between data entries and whether to move to the next data entry 
             * after each vertex or after each instance."
             * (https://vulkan-tutorial.com/Vertex_buffers/Vertex_input_description) */

            if(binding == 0)
                _vertex_input_bindings.push_back(createVertexInputBindingDescription(binding, total_stride, VK_VERTEX_INPUT_RATE_VERTEX));

            if(binding == 1)
                _vertex_input_bindings.push_back(createVertexInputBindingDescription(binding, total_stride, VK_VERTEX_INPUT_RATE_INSTANCE));

        }

        void Pipeline::addVertexAttribute(uint32_t binding, uint32_t location, uint32_t offset, VkFormat format) {
            /** "An attribute description struct describes how to extract a vertex attribute 
             * from a chunk of vertex data originating from a binding description"
             * (https://vulkan-tutorial.com/Vertex_buffers/Vertex_input_description)
             * @example a vertex binding could for example consist of the attributes vertex position and vertex color
             * @param format you can use the function translate() from formats.h to determine the correct vulkan format */

            _vertex_attributes.push_back(createVertexInputAttributeDescription(binding, location, offset, format));
        }


        void Pipeline::setInputAssembly(VkPrimitiveTopology topology) {
            /** @brief determines how primitives (i.e. triangles) are assembled from the vertices */

            _input_assembly_state = createPipelineInputAssembleStateCreateInfo(topology);
        }

        void Pipeline::setRasterizationState(bool enable_culling, VkFrontFace front_face, VkPolygonMode mode) {
            /** "Rasterization is the process by which a primitive is converted to a two-dimensional image" 
             * (https://registry.khronos.org/vulkan/site/spec/latest/chapters/primsrast.html)
             * @param enable_culling "culling" is a technique in which only triangles whose vertices appear in a specific order on the screen 
             * get rasterized (practically removing the back-faces (in the most common application of culling), which can improve performance )
             * @param front_face declare in which order the vertices have to appear to be spared from the culling 
             * @param mode choose between wireframe, points, or just "normal" fully rasterized triangles */

            _rasterization_state = createPipelineRasterizationStateCreateInfo(mode, enable_culling, front_face);
        }

        void Pipeline::setMultisampleState(bool enable_sample_shading) {
            /** @brief this function enables sample shading, which is not equal to multisampling, 
             * but may result in even better anti-aliasing
             * if you want to use multisampling, you cant render directly to the swapchain image anymore
             * instead you need to create an intermediate render target that supports more samples per pixel
             * and then resolve the multisample image to the visible one (via the renderpass) */
            
            _multisample_state = createPipelineMultisampleStateCreateInfo(enable_sample_shading);
        }

        void Pipeline::setBlending(uint32_t attachment, bool enable_blending, VkBlendOp color_blend_op, VkBlendFactor src_color_factor, VkBlendFactor dst_color_factor, VkBlendOp alpha_blend_op, VkBlendFactor src_alpha_factor, VkBlendFactor dst_alpha_factor) {
            /** @brief enable blending for the attachment (of the renderpass) ("mixing" colors after the fragment shader)
             * you can choose different blending methods for the color and alpha channels */

            // resizing the _blend_attachments vector so that the new blend state can be stored
            // initializing all other new blend attachments to no blending (until setBlending gets called for them)
            for(uint32_t i = _blend_attachments.size(); i <= attachment; i++) {
                _blend_attachments.push_back(createPipelineColorBlendAttachmentState(false, {}, {}, {}, {}, {}, {}));
            }

            // storing the blend attachment
            _blend_attachments.at(attachment) = createPipelineColorBlendAttachmentState(enable_blending, color_blend_op, src_color_factor, dst_color_factor, alpha_blend_op, src_alpha_factor, dst_alpha_factor);
        }

        void Pipeline::setShaderInput(const VkDescriptorSetLayout& layout, uint32_t slot) {
            /** @brief the descriptor set layout tells the pipeline which resources can be bound to the shaders
             * descriptor sets store descriptors, which are more or less pointers to resources like textures or uniform buffers
             * @param slot there can be more than one descriptor set for a shader, each set can be accessed via its slot id */

            if(slot >= _descriptor_set_layouts.size())
                _descriptor_set_layouts.resize(slot + 1);

            _descriptor_set_layouts.at(slot) = layout;
        }

        void Pipeline::setDepthStencilState(bool enable_depth_test, bool write_depth_values, VkCompareOp compare_op, bool enable_stencil_test) {
            /** @brief enable or disable depth testing and stencil testing */

            _depth_stencil_state = createPipelineDepthStencilStateCreateInfo(enable_depth_test, write_depth_values, compare_op, enable_stencil_test);
        }

        /////////////////////////////// init / cleanup ///////////////////////////////

        void Pipeline::init(const VkDevice& device, VkRenderPass render_pass, uint32_t subpass) {

            _device_handle = device;

            // creating structs that describe combined states
            _vertex_input_state = createPipelineVertexInputStateCreateInfo(_vertex_input_bindings, _vertex_attributes);
            _color_blend_state = createPipelineColorBlendStateCreateInfo(_blend_attachments);

            // creating the pipeline layout
            VkPipelineLayoutCreateInfo layout_info = createPipelineLayoutCreateInfo(_descriptor_set_layouts);
            vkCreatePipelineLayout(device, &layout_info, {}, &_layout);

            // joining all pipeline info structs
            VkGraphicsPipelineCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            info.pNext = nullptr;
            
            info.stageCount = _shader_stages.size();
            info.pStages = _shader_stages.data();
            info.pVertexInputState = &_vertex_input_state;
            info.pInputAssemblyState = &_input_assembly_state;
            info.pViewportState = &_viewport_state;
            info.pRasterizationState = &_rasterization_state;
            info.pMultisampleState = &_multisample_state;
            info.pColorBlendState = &_color_blend_state;
            info.pDepthStencilState = &_depth_stencil_state;
            info.renderPass = render_pass;
            info.subpass = subpass;
            info.basePipelineHandle = VK_NULL_HANDLE;
            info.layout = _layout;

            VK_ASSERT(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, {}, &_pipeline));

        }

        void Pipeline::cleanUp() {

            vkDestroyPipelineLayout(_device_handle, _layout, {});
            vkDestroyPipeline(_device_handle, _pipeline, {});
        }

        const VkViewport& Pipeline::getViewport() const {

            return _viewport;
        }

        const VkPipeline& Pipeline::getPipeline() const {

            return _pipeline;
        }

        const VkPipelineLayout& Pipeline::getPipelineLayout() const {
            
            return _layout;
        }

        const VkDescriptorSetLayout& Pipeline::getDescriptorSetLayout(uint32_t slot) const {

            assert(slot < _descriptor_set_layouts.size());

            return _descriptor_set_layouts.at(slot);
        }

        ///////////////////// creating pipeline related structs //////////////////////

        VkViewport Pipeline::createViewport(const VkExtent2D& extent) {

            VkViewport viewport{};
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = extent.width;
            viewport.height = extent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            return viewport;
        }

        VkRect2D Pipeline::createScissor(const VkExtent2D& extent) {

            VkRect2D scissor{};
            scissor.extent = extent;
            scissor.offset.x = 0;
            scissor.offset.y = 0;

            return scissor;
        }

        VkPipelineViewportStateCreateInfo Pipeline::createPipelineViewportStateCreateInfo(const VkViewport& viewport, const VkRect2D& scissor) {

            VkPipelineViewportStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.viewportCount = 1;
            info.pViewports = &viewport;
            info.scissorCount = 1;
            info.pScissors = &scissor;

            return info;
        }

        VkPipelineShaderStageCreateInfo Pipeline::createPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module) {
            // information about a shader stage (i.e. the vertex shader stage)

            VkPipelineShaderStageCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext = nullptr;
            info.stage = stage;
            info.module = module;
            info.pName = "main"; // entry point
            return info;
        }

        VkVertexInputBindingDescription Pipeline::createVertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate) {
            /** describes a buffer that provides vertex data
            * @param stride: total size of the per vertex data
            * @param input_rate: the data can be updated per vertex or per instance */

            VkVertexInputBindingDescription info{};
            info.binding = binding;
            info.inputRate = input_rate;
            info.stride = stride;

            return info;
        }

        VkVertexInputAttributeDescription Pipeline::createVertexInputAttributeDescription(uint32_t binding, uint32_t location, uint32_t offset, VkFormat format) {
            /** describes a single attribute of a vertex */

            VkVertexInputAttributeDescription info{};
            info.binding = binding;
            info.format = format;
            info.location = location;
            info.offset = offset;

            return info;
        }

        VkPipelineVertexInputStateCreateInfo Pipeline::createPipelineVertexInputStateCreateInfo(const std::vector<VkVertexInputBindingDescription>& vertex_input_bindings, const std::vector<VkVertexInputAttributeDescription>& vertex_input_attributes) {
            // combined information about how the vertex data gets acquired

            VkPipelineVertexInputStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.pVertexBindingDescriptions = vertex_input_bindings.data();
            info.vertexBindingDescriptionCount = vertex_input_bindings.size();
            info.pVertexAttributeDescriptions = vertex_input_attributes.data();
            info.vertexAttributeDescriptionCount = vertex_input_attributes.size();

            return info;
        }

        VkPipelineInputAssemblyStateCreateInfo Pipeline::createPipelineInputAssembleStateCreateInfo(VkPrimitiveTopology topology) {
            // info about how the vertices get assembled info primitives
            // like triangles, points, lines, ...

            VkPipelineInputAssemblyStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.topology = topology;
            info.primitiveRestartEnable = VK_FALSE;

            return info;
        }

        VkPipelineRasterizationStateCreateInfo Pipeline::createPipelineRasterizationStateCreateInfo(VkPolygonMode polygon_mode, bool enable_culling, VkFrontFace front_face) {
            // info about the rasterization stage
            // use VK_POLYGON_MODE_LINE for wireframe mode

            VkPipelineRasterizationStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.depthClampEnable = VK_FALSE;
            info.rasterizerDiscardEnable = VK_FALSE;
            info.polygonMode = polygon_mode;
            info.lineWidth = 1.0f;
            info.cullMode = enable_culling?VK_CULL_MODE_BACK_BIT:VK_CULL_MODE_NONE;
            info.frontFace = front_face;
            info.depthBiasEnable = VK_FALSE;
            info.depthBiasConstantFactor = 0.0f;
            info.depthBiasClamp = 0.0f;
            info.depthBiasSlopeFactor = 0.0f;

            return info;
        }

        VkPipelineMultisampleStateCreateInfo Pipeline::createPipelineMultisampleStateCreateInfo(bool enable_sample_shading) {

            VkPipelineMultisampleStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.sampleShadingEnable = enable_sample_shading;
            info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            info.minSampleShading = enable_sample_shading ? .2f : 1.0f;
            info.pSampleMask = nullptr;
            info.alphaToCoverageEnable = VK_FALSE;
            info.alphaToOneEnable = VK_FALSE;

            return info;
        }

        VkPipelineColorBlendAttachmentState Pipeline::createPipelineColorBlendAttachmentState(bool enable_blending, VkBlendOp color_blend_op, VkBlendFactor src_color_factor, VkBlendFactor dst_color_factor, VkBlendOp alpha_blend_op, VkBlendFactor src_alpha_factor, VkBlendFactor dst_alpha_factor) {
            // info about how to blend into an attachment

            VkPipelineColorBlendAttachmentState blend_state{};
            blend_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            blend_state.blendEnable = enable_blending;
            blend_state.colorBlendOp = color_blend_op;
            blend_state.alphaBlendOp = alpha_blend_op;
            blend_state.srcColorBlendFactor = src_color_factor;
            blend_state.dstColorBlendFactor = dst_color_factor;
            blend_state.srcAlphaBlendFactor = src_alpha_factor;
            blend_state.dstAlphaBlendFactor = dst_alpha_factor;

            return blend_state;
        }

        VkPipelineColorBlendStateCreateInfo Pipeline::createPipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& blend_attachments) {
            // combines the blend operations for all attachments

            VkPipelineColorBlendStateCreateInfo color_blending{};
            color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            color_blending.pNext = nullptr;
            color_blending.logicOpEnable = VK_FALSE;
            color_blending.logicOp = VK_LOGIC_OP_COPY;
            color_blending.attachmentCount = blend_attachments.size();
            color_blending.pAttachments = blend_attachments.data();

            return color_blending;
        }

        VkPipelineDepthStencilStateCreateInfo Pipeline::createPipelineDepthStencilStateCreateInfo(bool depth_test, bool write_depth_values, VkCompareOp compare_op, bool enable_stencil_test) {
            
            VkPipelineDepthStencilStateCreateInfo depth_stencil{};
            depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depth_stencil.depthTestEnable = depth_test;
            depth_stencil.depthWriteEnable = write_depth_values;
            depth_stencil.depthCompareOp = compare_op;
            depth_stencil.depthBoundsTestEnable = VK_FALSE;
            depth_stencil.stencilTestEnable = enable_stencil_test;

            return depth_stencil;
        }

        VkPipelineLayoutCreateInfo Pipeline::createPipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout>& layouts) {

            VkPipelineLayoutCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.pushConstantRangeCount = 0;
            info.pPushConstantRanges = nullptr;
            info.setLayoutCount = layouts.size();
            info.pSetLayouts = layouts.data();

            return info;
        }

    } // vulkan

} // undicht