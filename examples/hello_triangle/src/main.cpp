#include "debug.h"
#include "engine.h"
#include "window/glfw/window.h"
#include "core/vulkan/logical_device.h"
#include "core/vulkan/swap_chain.h"
#include "core/vulkan/frame_buffer.h"
#include "core/vulkan/render_pass.h"
#include "core/vulkan/pipeline.h"
#include "core/vulkan/shader_module.h"
#include "core/vulkan/command_buffer.h"
#include "core/vulkan/fence.h"
#include "core/vulkan/semaphore.h"

using namespace undicht;
using namespace graphics;
using namespace vulkan;

int main() {

    // initializing the engine and opening a window
    Engine undicht_engine;
    undicht_engine.init();
    Window glfw_window;
    glfw_window.open(undicht_engine.getVulkanInstance().getInstance(), "Hello Triangle!");
    LogicalDevice device;
    device.init(undicht_engine.getVulkanInstance().chooseGPU(), glfw_window.getSurface());

    // setting up the swap chain
    SwapChain swap_chain;
    swap_chain.init(device, glfw_window.getSurface());
    std::vector<ImageView> swap_images = swap_chain.retrieveSwapImages();

    // setting up the render pass
    RenderPass render_pass;
    render_pass.addAttachment(swap_chain.getSwapImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    render_pass.addSubPass({0}, {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
    render_pass.init(device.getDevice());

    // setting up the swap chain framebuffers
    std::vector<Framebuffer> swap_framebuffers(swap_images.size());
    for(int i = 0; i < swap_images.size(); i++) {
        swap_framebuffers.at(i).setAttachment(0, swap_images.at(i).getImageView());
        swap_framebuffers.at(i).init(device.getDevice(), render_pass, swap_chain.getExtent());
    }

    // setting up the shader modules
    ShaderModule vertex_shader;
    vertex_shader.init(device.getDevice(), VK_SHADER_STAGE_VERTEX_BIT, "../../../examples/hello_triangle/src/shader/bin/triangle.vert.spv");
    ShaderModule fragment_shader;
    fragment_shader.init(device.getDevice(), VK_SHADER_STAGE_FRAGMENT_BIT, "../../../examples/hello_triangle/src/shader/bin/triangle.frag.spv");

    // setting up the pipeline
    Pipeline pipeline;
    pipeline.addShaderModule(vertex_shader);
    pipeline.addShaderModule(fragment_shader);
    pipeline.setBlending(0, false);
    pipeline.setDepthStencilState(false);
    pipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipeline.setMultisampleState(false);
    pipeline.setRasterizationState(false);
    pipeline.setViewport(swap_chain.getExtent());
    pipeline.init(device.getDevice(), render_pass.getRenderPass());

    // setting up synchronisation objects
    Semaphore render_finished_semaphore;
    render_finished_semaphore.init(device.getDevice());
    Semaphore image_acquired_semaphore;
    image_acquired_semaphore.init(device.getDevice());
    Fence render_finished_fence;
    render_finished_fence.init(device.getDevice(), true);

    // setting up the command buffer
    CommandBuffer cmd_buffer;
    cmd_buffer.init(device.getDevice(), device.getGraphicsCmdPool());

    // render loop
    while(!glfw_window.shouldClose()) {
        
        // wait for previous frame to finish
        render_finished_fence.waitForProcessToFinish();

        // acquire image to render to
        int image_id = swap_chain.acquireNextSwapImage(image_acquired_semaphore.getAsSignal());

        if(image_id != -1) {
            // record command buffer
            VkClearValue clear_color = {0.2f, 0.2f, 0.2f, 0.0f};
            cmd_buffer.beginCommandBuffer(true);
            cmd_buffer.beginRenderPass(render_pass.getRenderPass(), swap_framebuffers.at(image_id).getFramebuffer(), swap_chain.getExtent(), {clear_color});
            cmd_buffer.bindGraphicsPipeline(pipeline.getPipeline());
            cmd_buffer.draw(3);
            cmd_buffer.endRenderPass();
            cmd_buffer.endCommandBuffer();

            // submit command buffer
            device.submitOnGraphicsQueue(cmd_buffer.getCommandBuffer(), render_finished_fence.getFence(), {image_acquired_semaphore.getAsWaitOn()}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {render_finished_semaphore.getAsSignal()});
            device.presentOnPresentQueue(swap_chain.getSwapchain(), image_id, {render_finished_semaphore.getAsWaitOn()});

        } else {
            // recreate the swap chain
            swap_chain.recreate(device, glfw_window.getSurface());
            swap_chain.freeSwapImages(swap_images);
            swap_images = swap_chain.retrieveSwapImages();
            for(int i = 0; i < swap_images.size(); i++) {
                swap_framebuffers.at(i).cleanUp();
                swap_framebuffers.at(i).setAttachment(0, swap_images.at(i).getImageView());
                swap_framebuffers.at(i).init(device.getDevice(), render_pass, swap_chain.getExtent());
            }
            pipeline.cleanUp();
            pipeline.setViewport(swap_chain.getExtent());
            pipeline.init(device.getDevice(), render_pass.getRenderPass());
            render_finished_fence.reset();
        }

        glfw_window.update();

    }

    // cleanup
    device.waitForProcessesToFinish();
    cmd_buffer.cleanUp();
    render_finished_semaphore.cleanUp();
    image_acquired_semaphore.cleanUp();
    render_finished_fence.cleanUp();
    pipeline.cleanUp();
    vertex_shader.cleanUp();
    fragment_shader.cleanUp();
    for(Framebuffer& f : swap_framebuffers) f.cleanUp();
    render_pass.cleanUp();
    swap_chain.freeSwapImages(swap_images);
    swap_chain.cleanUp();
    device.cleanUp();
    glfw_window.close();
    undicht_engine.cleanUp();

    return 0;
}