#include "debug.h"
#include "engine.h"
#include "application_templates/basic_app_template.h"

#include "scene/scene.h"
#include "scene_loader/scene_loader.h"
#include "scene/renderer/scene_renderer.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "profiler.h"

using namespace undicht;
using namespace graphics;
using namespace vulkan;
using namespace tools;

class App : public BasicAppTemplate {

  protected:

    SceneRenderer renderer;
    TransferBuffer transfer_buffer;
    Scene scene;

    // camera matrices
    glm::mat4 camera_view;
    glm::mat4 camera_proj;
    float rotation = 0.0f;

    // for calculating the frame time
    uint32_t last_frame_micros = micros();

  public:

    void init() {

        //BasicAppTemplate::init("Textured Cube", VK_PRESENT_MODE_FIFO_KHR); // vsync
        BasicAppTemplate::init("Textured Cube", VK_PRESENT_MODE_IMMEDIATE_KHR); // no vsync
        getWindow().setSize(1000, 800);
        setClearColor({0.01f, 0.0005f, 0.002f, 0.0f});
        Profiler::enableProfiler(true);

        UND_LOG << "initialized the app\n";

        // setting up the renderer
        renderer.init(getDevice(), _swap_chain, _vulkan_allocator);

        // setting up the transfer buffer
        transfer_buffer.init(_vulkan_allocator, {getDevice().getGraphicsQueueFamily()}, 50000000 * sizeof(float));

        scene.init();
        SceneLoader scene_loader;
        scene_loader.setInitObjects(getDevice(), _vulkan_allocator, transfer_buffer, renderer.getMaterialDescriptorCache(), renderer.getNodeDescriptorCache(), renderer.getMaterialSampler());
        scene_loader.importScene("res/tex_cube.dae", scene.addGroup("cube"));
        scene_loader.importScene("res/kos.dae", scene.addGroup("kos"));
        scene_loader.importScene("res/sponza/sponza.obj", scene.addGroup("sponza"));

        scene.getGroup("cube")->getRootNode().setLocalTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
        scene.updateGlobalTransformations();
        scene.updateNodeUBOs(transfer_buffer);

        // transferring the scene data
        CommandBuffer initial_transfer_cmd;
        initial_transfer_cmd.init(getDevice().getDevice(), getDevice().getGraphicsCmdPool());

        initial_transfer_cmd.beginCommandBuffer(true);
        transfer_buffer.completeTransfers(initial_transfer_cmd);
        transfer_buffer.reset();
        scene.genMipMaps(initial_transfer_cmd);
        initial_transfer_cmd.endCommandBuffer();

        getDevice().submitOnGraphicsQueue(initial_transfer_cmd.getCommandBuffer());
        getDevice().waitForProcessesToFinish();
        initial_transfer_cmd.cleanUp();
    }

    void cleanUp() {

        Profiler::writeToCsvFile("./profile.csv");

        getDevice().waitForProcessesToFinish();

        transfer_buffer.cleanUp();
        renderer.cleanUp(_swap_chain);
        scene.cleanUp();
        BasicAppTemplate::cleanUp();
    }

    void recreateSwapChain() {

        BasicAppTemplate::recreateSwapChain();
        renderer.recreateFramebuffers(_vulkan_allocator, getSwapChain());
        
    }

    void framePreperation() {
        // called while the old frame is still being processed on the gpu

        // update the camera rotation
        rotation += (micros() - last_frame_micros) / 3000000.0f;
        last_frame_micros = micros();

        camera_view = glm::lookAt(glm::vec3(5.0f * glm::sin(rotation), 2.0, 5.0f * glm::cos(rotation)), glm::vec3(0.0, 2.5f, 0.0), glm::vec3(0.0, 1.0, 0.0));
        camera_proj = glm::perspective(90.0f, float(getWindow().getWidth()) / getWindow().getHeight(), 0.1f, 2000.0f);            
        camera_proj[1][1] *= -1; // since -y is up in vulkan ndc
        
    }

    void transferCommands() {
        // called after the previous frames transfer commands have finished

        transfer_buffer.reset();

        renderer.loadCameraMatrices(glm::value_ptr(camera_view), glm::value_ptr(camera_proj), transfer_buffer);
        transfer_buffer.completeTransfers(getTransferCmd());

    }

    void drawCommands(int swap_image_id) {
        // record draw commands here

        Profiler p;
        p.start("  renderer.begin");
        renderer.begin(getDrawCmd(), swap_image_id);
        p.start("  renderer.draw");
        renderer.draw(getDrawCmd(), scene);
        p.start("  renderer.end");
        renderer.end(getDrawCmd());
    }

    void frameFinalization() {
        // called after the previous frame has finished rendering

    }

};

int main() {

    App app;
    app.init();

    uint32_t start_time = millis();
    uint32_t frame_count = 0;

    while(app.run()) {

        if(frame_count == 100) {
            UND_LOG << "FPS: " << 100.0f * 1000.0f / (millis() - start_time) << "\n";
            frame_count = 0;
            start_time = millis();
            Profiler::enableProfiler(false);
        }

        frame_count++;
    }


    app.cleanUp();

    return 0;
}