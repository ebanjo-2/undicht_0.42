#ifndef SKY_RENDERER_H
#define SKY_RENDERER_H

#include "renderer/renderer_templates/basic_renderer_template.h"

class SkyRenderer : public undicht::vulkan::BasicRendererTemplate {

  protected:
  
    VkDescriptorSetLayout _sky_descriptor_layout;

  public:

    void init(VkDevice device, VkRenderPass render_pass, VkExtent2D view_port);
    void cleanUp();

    void begin(undicht::vulkan::CommandBuffer &draw_cmd);

    /// @brief draws the meshes of the node, does not draw child nodes
    /// @return the number of draw calls that were made
    uint32_t draw(undicht::vulkan::CommandBuffer &cmd);

  protected:
    // functions to initialize parts of the renderer

    virtual void initShaderModules();

};

#endif // SKY_RENDERER_H