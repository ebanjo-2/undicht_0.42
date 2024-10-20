#include "sky_renderer.h"
#include "file_tools.h"

void SkyRenderer::init(VkDevice device, VkRenderPass render_pass, VkExtent2D view_port) {

    BasicRendererTemplate::init(device, render_pass, view_port);

    initShaderModules();
    initPipeLine(view_port);

}

void SkyRenderer::cleanUp() {

}

void SkyRenderer::begin(undicht::vulkan::CommandBuffer &draw_cmd) {
    
}

uint32_t SkyRenderer::draw(undicht::vulkan::CommandBuffer &cmd) {
    /// @brief draws the meshes of the node, does not draw child nodes
    /// @return the number of draw calls that were made
}

void SkyRenderer::initShaderModules() {

    std::string directory = undicht::getFilePath(UND_CODE_SRC_FILE);

    _vertex_shader.init(_device_handle, VK_SHADER_STAGE_VERTEX_BIT, directory + "/shader/bin/sky_shader.vert.spv");
    _fragment_shader.init(_device_handle, VK_SHADER_STAGE_FRAGMENT_BIT, directory + "/shader/bin/sky_shader.frag.spv");
}
