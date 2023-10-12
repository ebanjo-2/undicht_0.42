#include "debug.h"
#include "engine.h"
#include "application.h"
#include "frame_manager.h"

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

#include "profiler.h"

using namespace undicht;
using namespace graphics;
using namespace vulkan;
using namespace tools;

int main() {

    // initializing the engine and opening a window
    Application app;
    app.init("Textured cube", VK_PRESENT_MODE_IMMEDIATE_KHR);
    //app.init("Textured cube", VK_PRESENT_MODE_FIFO_KHR);
    Profiler::enableProfiler(true);

    UND_LOG << "initialized the app\n";
    
    // setting up the vulkan memory allocator
    vma::VulkanMemoryAllocator vulkan_allocator;
    vulkan_allocator.init(app.getVulkanInstance().getInstance(), app.getDevice().getDevice(), app.getDevice().getPhysicalDevice());

    // setting up the renderer
    SceneRenderer renderer;
    renderer.init(app.getDevice(), app.getSwapChain(), vulkan_allocator);

    // buffer and command buffer for loading data
    CommandBuffer transfer_command;
    transfer_command.init(app.getDevice().getDevice(), app.getDevice().getGraphicsCmdPool());
    TransferBuffer transfer_buffer;
    transfer_buffer.init(vulkan_allocator, {app.getDevice().getGraphicsQueueFamily()}, 50000000 * sizeof(float));
    Fence transfer_finished;
    transfer_finished.init(app.getDevice().getDevice(), false);

    // loading the cube model using assimp
    Scene scene;
    scene.init(app.getDevice(), vulkan_allocator, renderer.getNodeDescriptorCache());
    SceneLoader scene_loader;
    //scene_loader.importScene("res/tex_cube.dae", scene, transfer_buffer, renderer.getMaterialDescriptorCache(), renderer.getMaterialSampler(), app.getDevice(), vulkan_allocator);
    //scene_loader.importScene("res/sponza_collada/sponza.dae", scene, transfer_buffer);
    scene_loader.importScene("res/sponza/sponza.obj", scene, transfer_buffer, renderer.getMaterialDescriptorCache(), renderer.getNodeDescriptorCache(), renderer.getMaterialSampler(), app.getDevice(), vulkan_allocator);
    //scene_loader.importScene("res/billiard/sphere.dae", scene, transfer_buffer, renderer.getMaterialDescriptorCache(), renderer.getNodeDescriptorCache(), renderer.getMaterialSampler(), app.getDevice(), vulkan_allocator);

    // matrices for the camera (updated every frame)
    glm::mat4 camera_view;
    glm::mat4 camera_proj;
    float rotation = 0.0f;

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

    uint32_t last_fps_millis = millis();
    uint32_t last_frame_micros = micros();
    uint32_t num_frames_since_fps = 0;

    FrameManager frames;
    frames.init(app.getDevice());

    // render loop
    while(!app.getWindow().shouldClose()) {

        Profiler main_loop("main loop");

        // update the camera rotation
        rotation += (micros() - last_frame_micros) / 3000000.0f;
        last_frame_micros = micros();

        // acquire next swapchain image
        int image_id = frames.prepareNextFrame(app.getSwapChain());

        if(image_id != -1) {

            // record draw command
            if(!frames.getDrawCmd().isReady()) {
                PROFILE_SCOPE("recording command buffer",
                frames.getDrawCmd().resetCommandBuffer();
                frames.getDrawCmd().beginCommandBuffer(false);
                renderer.begin(frames.getDrawCmd(), image_id);
                renderer.draw(frames.getDrawCmd(), scene);
                renderer.end(frames.getDrawCmd());
                frames.getDrawCmd().endCommandBuffer();
                )
            }

            // update the camera matrices
            PROFILE_SCOPE("updating camera matrices",
            camera_view = glm::lookAt(glm::vec3(5.0f * glm::sin(rotation), 2.0, 5.0f * glm::cos(rotation)), glm::vec3(0.0, 2.5f, 0.0), glm::vec3(0.0, -1.0, 0.0));
            camera_proj = glm::perspective(90.0f, float(app.getWindow().getWidth()) / app.getWindow().getHeight(), 0.1f, 2000.0f);            
            )

            // prepare for reusing the transfer buffer
            PROFILE_SCOPE("waiting for transfer finished",
            frames.waitForTransferFinished();
            transfer_buffer.reset();
            )

            // stage the matrix data in the transfer buffer
            PROFILE_SCOPE("staging matrix data / recording transfer command",
            renderer.loadCameraMatrices(glm::value_ptr(camera_view), glm::value_ptr(camera_proj), transfer_buffer);
            frames.getTransferCmd().resetCommandBuffer();
            frames.getTransferCmd().beginCommandBuffer(true);
            transfer_buffer.completeTransfers(frames.getTransferCmd());
            frames.getTransferCmd().endCommandBuffer();
            )

            // wait for previous frame to finish rendering before updating the uniform buffer
            PROFILE_SCOPE("waiting for previous render finished",
            frames.finishLastFrame();
            )

            // submit current draw command
            PROFILE_SCOPE("submitting frame",
            frames.submitFrame(app.getDevice(), app.getSwapChain(), true);
            )

        } else {

            // recreate the swap chain
            app.recreateSwapChain(VK_PRESENT_MODE_IMMEDIATE_KHR);
            //app.recreateSwapChain(VK_PRESENT_MODE_FIFO_KHR);
            renderer.recreateFramebuffers(vulkan_allocator, app.getSwapChain());
            frames.reset();

        }

        if(num_frames_since_fps > 1000) {
            num_frames_since_fps = 0;
            UND_LOG << "FPS: " << 1000.0f * 1000.0f / (millis() - last_fps_millis) << "\n";
            last_fps_millis = millis();
            Profiler::enableProfiler(false); // only record first 1000 frames
        }

        num_frames_since_fps++;

        PROFILE_SCOPE("update window",
            app.getWindow().update();
        )

    }

    // save performance data
    Profiler::writeToCsvFile("performance.csv");

    // cleanup
    app.getDevice().waitForProcessesToFinish();

    transfer_finished.cleanUp();
    transfer_command.cleanUp();
    transfer_buffer.cleanUp();

    frames.cleanUp();
    renderer.cleanUp(app.getSwapChain());
    scene.cleanUp();
    vulkan_allocator.cleanUp();

    app.cleanUp();

    return 0;
}