#include "debug.h"
#include "engine.h"
#include "application.h"

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
    Application app;
    app.init("Hello Triangle!", VK_PRESENT_MODE_FIFO_KHR);

    // setting up the render pass
    RenderPass render_pass;
    render_pass.addAttachment(app.getSwapChain().getSwapImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    render_pass.addSubPass({0}, {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
    render_pass.init(app.getDevice().getDevice());

    // setting up the swap chain framebuffers
    std::vector<Framebuffer> swap_framebuffers = app.createSwapImageFramebuffers(render_pass);

    // setting up the shader modules
    ShaderModule vertex_shader;
    vertex_shader.init(app.getDevice().getDevice(), VK_SHADER_STAGE_VERTEX_BIT, "src/shader/bin/triangle.vert.spv");
    ShaderModule fragment_shader;
    fragment_shader.init(app.getDevice().getDevice(), VK_SHADER_STAGE_FRAGMENT_BIT, "src/shader/bin/triangle.frag.spv");

    // setting up the pipeline
    Pipeline pipeline;
    pipeline.addShaderModule(vertex_shader);
    pipeline.addShaderModule(fragment_shader);
    pipeline.setBlending(0, false);
    pipeline.setDepthStencilState(false);
    pipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipeline.setMultisampleState(false);
    pipeline.setRasterizationState(false);
    pipeline.setViewport(app.getSwapChain().getExtent());
    pipeline.init(app.getDevice().getDevice(), render_pass.getRenderPass());

    // setting up synchronisation objects
    Semaphore render_finished_semaphore;
    render_finished_semaphore.init(app.getDevice().getDevice());
    Semaphore image_acquired_semaphore;
    image_acquired_semaphore.init(app.getDevice().getDevice());
    Fence render_finished_fence;
    render_finished_fence.init(app.getDevice().getDevice(), true);

    // setting up the command buffer
    CommandBuffer cmd_buffer;
    cmd_buffer.init(app.getDevice().getDevice(), app.getDevice().getGraphicsCmdPool());

    // render loop
    while(!app.getWindow().shouldClose()) {
        
        // wait for previous frame to finish
        render_finished_fence.waitForProcessToFinish();

        // acquire image to render to
        int image_id = app.getSwapChain().acquireNextSwapImage(image_acquired_semaphore.getAsSignal());

        if(image_id != -1) {
            // record command buffer
            VkClearValue clear_color = {0.2f, 0.2f, 0.2f, 0.0f};
            cmd_buffer.beginCommandBuffer(true);
            cmd_buffer.beginRenderPass(render_pass.getRenderPass(), swap_framebuffers.at(image_id).getFramebuffer(), app.getSwapChain().getExtent(), {clear_color});
            cmd_buffer.bindGraphicsPipeline(pipeline.getPipeline());
            cmd_buffer.draw(3);
            cmd_buffer.endRenderPass();
            cmd_buffer.endCommandBuffer();

            // submit command buffer
            app.getDevice().submitOnGraphicsQueue(cmd_buffer.getCommandBuffer(), render_finished_fence.getFence(), {image_acquired_semaphore.getAsWaitOn()}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {render_finished_semaphore.getAsSignal()});
            app.getDevice().presentOnPresentQueue(app.getSwapChain().getSwapchain(), image_id, {render_finished_semaphore.getAsWaitOn()});

        } else {
            // recreate the swap chain
            app.recreateSwapChain(VK_PRESENT_MODE_FIFO_KHR);
            app.recreateSwapImageFramebuffers(swap_framebuffers, render_pass);

            pipeline.cleanUp();
            pipeline.setViewport(app.getSwapChain().getExtent());
            pipeline.init(app.getDevice().getDevice(), render_pass.getRenderPass());
            render_finished_fence.reset();
        }

        app.getWindow().update();

    }

    // cleanup
    app.getDevice().waitForProcessesToFinish();
    cmd_buffer.cleanUp();
    render_finished_semaphore.cleanUp();
    image_acquired_semaphore.cleanUp();
    render_finished_fence.cleanUp();
    pipeline.cleanUp();
    vertex_shader.cleanUp();
    fragment_shader.cleanUp();
    app.destroySwapImageFramebuffers(swap_framebuffers);
    render_pass.cleanUp();

    app.cleanUp();

    return 0;
}