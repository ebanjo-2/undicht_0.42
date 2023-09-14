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
#include "core/vulkan/buffer.h"

#include "vulkan_memory_allocator.h"

#include "scene/scene.h"
#include "scene_loader/scene_loader.h"
#include "scene/renderer/scene_renderer.h"

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace undicht;
using namespace graphics;
using namespace vulkan;
using namespace tools;

int main() {

    // initializing the engine and opening a window
    Application app;
    app.init("Textured cube", VK_PRESENT_MODE_IMMEDIATE_KHR);

    UND_LOG << "initialized the app\n";
    
    // setting up the vulkan memory allocator
    vma::VulkanMemoryAllocator vulkan_allocator;
    vulkan_allocator.init(app.getVulkanInstance().getInstance(), app.getDevice().getDevice(), app.getDevice().getPhysicalDevice());

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

    // setting up the renderer
    SceneRenderer renderer;
    renderer.init(app.getDevice(), app.getSwapChain(), vulkan_allocator);

    // buffer and command buffer for loading data
    CommandBuffer transfer_command;
    transfer_command.init(app.getDevice().getDevice(), app.getDevice().getGraphicsCmdPool());
    TransferBuffer transfer_buffer;
    transfer_buffer.init(vulkan_allocator, {app.getDevice().getGraphicsQueueFamily()}, 100000000 * sizeof(float));
    Fence transfer_finished;
    transfer_finished.init(app.getDevice().getDevice(), false);

    // loading the cube model using assimp
    Scene scene;
    scene.init(app.getDevice(), vulkan_allocator);
    SceneLoader scene_loader;
    //scene_loader.importScene("res/tex_cube.dae", scene, transfer_buffer);
    //scene_loader.importScene("res/sponza_collada/sponza.dae", scene, transfer_buffer);
    scene_loader.importScene("res/sponza/sponza.obj", scene, transfer_buffer);

    // setting up the camera matrices
    glm::mat4 camera_view = glm::lookAt(glm::vec3(4.0, 2.0, 4.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
    glm::mat4 camera_proj = glm::perspective(90.0f, 800.0f / 600.0f, 0.1f, 1000.0f);

    renderer.loadCameraMatrices(glm::value_ptr(camera_view), glm::value_ptr(camera_proj), transfer_buffer);

    // submit the transfer command
    transfer_command.beginCommandBuffer(true);
    transfer_buffer.completeTransfers(transfer_command);
    transfer_command.endCommandBuffer();
    app.getDevice().submitOnGraphicsQueue(transfer_command.getCommandBuffer(), transfer_finished.getFence());
    transfer_finished.waitForProcessToFinish();

    UND_LOG << "transfered all the data\n";

    // generate mip maps (reusing the transfer command buffer)
    transfer_command.beginCommandBuffer(true);
    scene.genMipMaps(transfer_command);
    transfer_command.endCommandBuffer();
    app.getDevice().submitOnGraphicsQueue(transfer_command.getCommandBuffer(), transfer_finished.getFence());
    transfer_finished.waitForProcessToFinish();

    UND_LOG << "generated all the mip maps\n";

    // clean up transfer objects
    transfer_finished.cleanUp();
    transfer_command.cleanUp();
    transfer_buffer.cleanUp();

    uint32_t last_millis = millis();
    uint32_t num_frames_since_fps = 0;

    // render loop
    while(!app.getWindow().shouldClose()) {

        if(num_frames_since_fps > 100) {
            num_frames_since_fps = 0;
            UND_LOG << "FPS: " << 100.0f * 1000.0f / (millis() - last_millis) << "\n";
            last_millis = millis();
        }

        num_frames_since_fps++;

        // wait for previous frame to finish
        render_finished_fence.waitForProcessToFinish();

        // acquire image to render to
        int image_id = app.getSwapChain().acquireNextSwapImage(image_acquired_semaphore.getAsSignal());

        if(image_id != -1) {

            // record command buffer
            cmd_buffer.beginCommandBuffer(true);
            renderer.begin(cmd_buffer, image_acquired_semaphore, image_id);
            renderer.draw(cmd_buffer, scene);
            renderer.end(cmd_buffer);
            cmd_buffer.endCommandBuffer();

            // submit command buffer
            app.getDevice().submitOnGraphicsQueue(cmd_buffer.getCommandBuffer(), render_finished_fence.getFence(), {image_acquired_semaphore.getAsWaitOn()}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {render_finished_semaphore.getAsSignal()});
            app.getDevice().presentOnPresentQueue(app.getSwapChain().getSwapchain(), image_id, {render_finished_semaphore.getAsWaitOn()});

        } else {
            // recreate the swap chain
            app.recreateSwapChain(VK_PRESENT_MODE_IMMEDIATE_KHR);

            renderer.recreateFramebuffers(vulkan_allocator, app.getSwapChain());
            render_finished_fence.reset();

            // update aspect ratio
            camera_proj = glm::perspective(90.0f, app.getWindow().getWidth() / float(app.getWindow().getHeight()), 0.1f, 1000.0f);
            //renderer.loadCameraMatrices(glm::value_ptr(camera_view), glm::value_ptr(camera_proj));

        }

        app.getWindow().update();

    }

    // cleanup
    app.getDevice().waitForProcessesToFinish();

    renderer.cleanUp(app.getSwapChain());
    scene.cleanUp();
    vulkan_allocator.cleanUp();
    cmd_buffer.cleanUp();
    render_finished_semaphore.cleanUp();
    image_acquired_semaphore.cleanUp();
    render_finished_fence.cleanUp();

    app.cleanUp();

    return 0;
}