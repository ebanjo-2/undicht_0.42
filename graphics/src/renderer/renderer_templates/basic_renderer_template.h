#ifndef BASIC_RENDERER_TEMPLATE_H
#define BASIC_RENDERER_TEMPLATE_H

#include "core/vulkan/logical_device.h"
#include "core/vulkan/pipeline.h"
#include "core/vulkan/shader_module.h"
#include "core/vulkan/descriptor_set_layout.h"
#include "core/vulkan/render_pass.h"
#include "core/vulkan/command_buffer.h"
#include "core/vulkan/sampler.h"

#include "renderer/vulkan/descriptor_set_cache.h"

#include <vector>
#include "string"

namespace undicht {

    namespace vulkan {

        class BasicRendererTemplate {
            /** a class that implements functionality commonly used by Basic Vulkan Renderers 
             * the functions provided may be used, integrated or overwritten by derived Renderers */
          protected:

            VkDevice _device_handle;
            VkRenderPass _render_pass_handle;

            vulkan::Pipeline _pipeline;
            vulkan::ShaderModule _vertex_shader;
            vulkan::ShaderModule _fragment_shader;

          public:

            virtual void init(VkDevice device, VkRenderPass render_pass, VkExtent2D view_port);
            virtual void cleanUp();

            virtual void setViewPort(VkExtent2D view_port);
            
            virtual void begin(vulkan::CommandBuffer& draw_cmd);
            virtual void end(vulkan::CommandBuffer& draw_cmd);

        protected:
            // functions to initialize parts of the renderer
            // (to change the initialization overwrite the function)

            virtual void initShaderModules();
            virtual void initPipeLine(VkExtent2D view_port);
            
            // functions that initialize parts of the pipeline 
            // (to change the initialization overwrite the function)
            virtual void setShaderInput();
            virtual void setVertexAttributes();
            virtual void setRasterizationState();
            virtual void addShaderModules();
            virtual void setBlending();
            virtual void setDepthStencilState();
            virtual void setMultisampleState();

        };

    } // vulkan

} // undicht

#endif // BASIC_RENDERER_TEMPLATE_H