#ifndef BASIC_RENDERER
#define BASIC_RENDERER

#include "scene/scene.h"

#include "core/vulkan/logical_device.h"
#include "core/vulkan/pipeline.h"
#include "core/vulkan/shader_module.h"
#include "core/vulkan/descriptor_set_layout.h"
#include "renderer/vulkan/descriptor_set_cache.h"
#include "core/vulkan/render_pass.h"
#include "core/vulkan/command_buffer.h"
#include "core/vulkan/sampler.h"

namespace undicht {

    namespace graphics {

        class BasicRenderer {

          protected:

            VkDevice _device_handle;
            VkRenderPass _render_pass_handle;

            vulkan::Pipeline _pipeline;
            vulkan::ShaderModule _vertex_shader;
            vulkan::ShaderModule _fragment_shader;
            vulkan::Sampler _sampler;

          public:

            void init(VkDevice device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_layout, VkDescriptorSetLayout material_descriptor_layout, VkDescriptorSetLayout node_descriptor_layout, VkExtent2D view_port);
            void cleanUp();

            void setViewPort(VkExtent2D view_port);

            void begin(vulkan::CommandBuffer& draw_cmd, VkDescriptorSet global_descriptor_set);
            void end(vulkan::CommandBuffer& draw_cmd);

            /// @brief draws the meshes of the node, does not draw child nodes
            /// @return the number of draw calls that were made
            uint32_t draw(vulkan::CommandBuffer& cmd, Scene& scene, Node& node);

          protected:
            // functions to initialize parts of the renderer

            void initShaderModules();
            void initPipeLine(VkExtent2D view_port, VkDescriptorSetLayout global_descriptor_layout, VkDescriptorSetLayout material_descriptor_layout, VkDescriptorSetLayout& node_descriptor_layout, VkRenderPass render_pass);

        };

    } // graphics

} // undicht

#endif // BASIC_RENDERER