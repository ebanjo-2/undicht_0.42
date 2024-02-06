#ifndef BASIC_ANIMATION_RENDERER_H
#define BASIC_ANIMATION_RENDERER_H

#include "scene/scene.h"
#include "renderer/renderer_templates/basic_renderer_template.h"


namespace undicht {

    namespace graphics {

        class BasicAnimationRenderer : public vulkan::BasicRendererTemplate {

          protected:

            VkDescriptorSetLayout _global_descriptor_layout;
            VkDescriptorSetLayout _material_descriptor_layout;
            VkDescriptorSetLayout _node_descriptor_layout;

          public:

            void init(VkDevice device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_layout, VkDescriptorSetLayout material_descriptor_layout, VkDescriptorSetLayout node_descriptor_layout, VkExtent2D view_port);
            void cleanUp();

            void begin(vulkan::CommandBuffer& draw_cmd, VkDescriptorSet global_descriptor_set);

            /// @brief draws the meshes of the node, does not draw child nodes
            /// @return the number of draw calls that were made
            uint32_t draw(vulkan::CommandBuffer& cmd, SceneGroup& scene, Node& node);

          protected:
            // functions to initialize parts of the renderer

            virtual void initShaderModules();

            // functions that initialize parts of the pipeline 
            virtual void setShaderInput();
            virtual void setVertexAttributes();
            virtual void setRasterizationState();
            virtual void addShaderModules();
            virtual void setBlending();
            virtual void setDepthStencilState();
            virtual void setMultisampleState();
            
        };

    } // graphics

} // undicht

#endif // BASIC_ANIMATION_RENDERER_H