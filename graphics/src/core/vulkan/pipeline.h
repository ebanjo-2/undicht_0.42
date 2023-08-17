#ifndef PIPELINE_H
#define PIPELINE_H

#include "vector"

#include "vulkan/vulkan.h"

#include "shader_module.h"
#include "buffer_layout.h"

namespace undicht {

    namespace vulkan {

        class Pipeline {
            /** "The VkPipeline is a huge object in Vulkan that encompasses the configuration of the entire GPU for the draw. 
             * Building them can be very expensive, as it will fully convert the shader module into the GPU instructions, 
             * and will validate the setup for it" 
             * (https://vkguide.dev/docs/chapter-2/pipeline_walkthrough/) */
          protected:

            VkDevice _device_handle;
            VkPipeline _pipeline;

            // configuration of the pipeline
            VkViewport _viewport;
            VkRect2D _scissor;
            VkPipelineViewportStateCreateInfo _viewport_state;
            std::vector<VkPipelineShaderStageCreateInfo> _shader_stages;
            std::vector<VkVertexInputBindingDescription> _vertex_input_bindings;
            std::vector<VkVertexInputAttributeDescription> _vertex_attributes;
            VkPipelineVertexInputStateCreateInfo _vertex_input_state;
            VkPipelineInputAssemblyStateCreateInfo _input_assembly_state;
            VkPipelineRasterizationStateCreateInfo _rasterization_state;
            VkPipelineMultisampleStateCreateInfo _multisample_state;
            std::vector<VkPipelineColorBlendAttachmentState> _blend_attachments;
            VkPipelineColorBlendStateCreateInfo _color_blend_state;
            VkPipelineDepthStencilStateCreateInfo _depth_stencil_state;            
            VkPipelineLayout _layout; // contains info about the input to the shaders (ubo and texture bindings, push constants)
            std::vector<VkDescriptorSetLayout> _descriptor_set_layouts;
        
          public:
            // functions to configure the pipeline (has to be completly done before init is called)

            void setViewport(const VkExtent2D& extent);

            void addShaderModule(const ShaderModule& module);

            /** "A vertex binding describes at which rate to load data from memory throughout the vertices. 
             * It specifies the number of bytes between data entries and whether to move to the next data entry 
             * after each vertex or after each instance."
             * (https://vulkan-tutorial.com/Vertex_buffers/Vertex_input_description) */
            void addVertexBinding(uint32_t binding, uint32_t total_stride);

            /** "An attribute description struct describes how to extract a vertex attribute 
             * from a chunk of vertex data originating from a binding description"
             * (https://vulkan-tutorial.com/Vertex_buffers/Vertex_input_description)
             * @example a vertex binding could for example consist of the attributes vertex position and vertex color
             * @param format you can use the function translate() from formats.h to determine the correct vulkan format */
            void addVertexAttribute(uint32_t binding, uint32_t location, uint32_t offset, VkFormat format);

            /** @brief determines how primitives (i.e. triangles) are assembled from the vertices */
            void setInputAssembly(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

            /** "Rasterization is the process by which a primitive is converted to a two-dimensional image" 
             * (https://registry.khronos.org/vulkan/site/spec/latest/chapters/primsrast.html)
             * @param enable_culling "culling" is a technique in which only triangles whose vertices appear in a specific order on the screen 
             * get rasterized (practically removing the back-faces (in the most common application of culling), which can improve performance )
             * @param front_face declare in which order the vertices have to appear to be spared from the culling 
             * @param mode choose between wireframe (lines), points, or just "normal" fully rasterized triangles */
            void setRasterizationState(bool enable_culling, VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE, VkPolygonMode mode = VK_POLYGON_MODE_FILL);
            
            /** @brief this function enables sample shading, which is not equal to multisampling, 
             * but may result in even better anti-aliasing (https://vulkan-tutorial.com/Multisampling)
             * if you want to use multisampling, you cant render directly to the swapchain image anymore
             * instead you need to create an intermediate render target that supports more samples per pixel
             * and then resolve the multisample image to the visible one (via the renderpass) */
            void setMultisampleState(bool enable_sample_shading = false);

            /** @brief enable blending for the attachment (of the renderpass) ("mixing" colors after the fragment shader)
             * you can choose different blending methods for the color and alpha channels */
            void setBlending(uint32_t attachment, bool enable_blending = false, VkBlendOp color_blend_op = {}, VkBlendFactor src_color_factor = {}, VkBlendFactor dst_color_factor = {}, VkBlendOp alpha_blend_op = {}, VkBlendFactor src_alpha_factor = {}, VkBlendFactor dst_alpha_factor = {});

            /** @brief the descriptor set layout tells the pipeline which resources can be bound to the shaders
             * descriptor sets store descriptors, which are more or less pointers to resources like textures or uniform buffers
             * @param slot there can be more than one descriptor set for a shader, each set can be accessed via its slot id */
            void setShaderInput(const VkDescriptorSetLayout& layout, uint32_t slot = 0);

            /** @brief enable or disable depth testing and stencil testing */
            void setDepthStencilState(bool enable_depth_test, bool write_depth_values = true, VkCompareOp compare_op = VK_COMPARE_OP_LESS, bool enable_stencil_test = false);


          public:
            // init / cleanup

            void init(const VkDevice& device, VkRenderPass render_pass, uint32_t subpass = 0);
            void cleanUp();

            const VkViewport& getViewport() const;
            const VkPipeline& getPipeline() const;
            const VkPipelineLayout& getPipelineLayout() const;
            const VkDescriptorSetLayout& getDescriptorSetLayout(uint32_t slot = 0) const;

          protected:
            // creating pipeline related structs

            VkViewport static createViewport(const VkExtent2D& extent);
            VkRect2D static createScissor(const VkExtent2D& extent);
            VkPipelineViewportStateCreateInfo static createPipelineViewportStateCreateInfo(const VkViewport& viewport, const VkRect2D& scissor);
            VkPipelineShaderStageCreateInfo static createPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module);
            VkVertexInputBindingDescription static createVertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate = VK_VERTEX_INPUT_RATE_VERTEX);
            VkVertexInputAttributeDescription static createVertexInputAttributeDescription(uint32_t binding, uint32_t location, uint32_t offset, VkFormat format);
            VkPipelineVertexInputStateCreateInfo static createPipelineVertexInputStateCreateInfo(const std::vector<VkVertexInputBindingDescription>& vertex_input_bindings, const std::vector<VkVertexInputAttributeDescription>& vertex_input_attributes);
            VkPipelineInputAssemblyStateCreateInfo static createPipelineInputAssembleStateCreateInfo(VkPrimitiveTopology topology);
            VkPipelineRasterizationStateCreateInfo static createPipelineRasterizationStateCreateInfo(VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL, bool enable_culling = false, VkFrontFace front_face = VK_FRONT_FACE_CLOCKWISE);
            VkPipelineMultisampleStateCreateInfo static createPipelineMultisampleStateCreateInfo(bool enable_sample_shading);
            VkPipelineColorBlendAttachmentState static createPipelineColorBlendAttachmentState(bool enable_blending, VkBlendOp color_blend_op, VkBlendFactor src_color_factor, VkBlendFactor dst_color_factor, VkBlendOp alpha_blend_op, VkBlendFactor src_alpha_factor, VkBlendFactor dst_alpha_factor);
            VkPipelineColorBlendStateCreateInfo static createPipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& blend_attachments);
            VkPipelineDepthStencilStateCreateInfo static createPipelineDepthStencilStateCreateInfo(bool depth_test, bool write_depth_values, VkCompareOp compare_op, bool enable_stencil_test);
            VkPipelineLayoutCreateInfo static createPipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout>& layouts = {});

        };

    } // vulkan

} // undicht

#endif // PIPELINE_H