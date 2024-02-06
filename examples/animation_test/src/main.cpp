#include "application_templates/basic_app_template.h"
#include "renderer/vulkan/transfer_buffer.h"
#include "scene/renderer/scene_renderer.h"
#include "scene/scene.h"
#include "scene_loader/scene_loader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/exponential.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace undicht;
using namespace graphics;
using namespace vulkan;
using namespace tools;

class AnimationTest : public BasicAppTemplate {

  protected:

    CommandBuffer _load_cmd_buffer;
    TransferBuffer _transfer_buffer;
    SceneRenderer _renderer;
    Scene _scene;

  public:

    void init() {

        BasicAppTemplate::init("Animation Test", VK_PRESENT_MODE_FIFO_KHR);
        getWindow().setSize(1000, 800);
        //setClearColor({0.01f, 0.0005f, 0.002f, 0.0f});

        _load_cmd_buffer.init(getDevice().getDevice(), getDevice().getGraphicsCmdPool());
        _load_cmd_buffer.beginCommandBuffer(true);

        _transfer_buffer.init(_vulkan_allocator, {getDevice().getGraphicsQueueFamily()}, 10000000); // reserve 10 Mb
        _renderer.init(getDevice(), getSwapChain(), _vulkan_allocator);

        SceneLoader loader;
        loader.setInitObjects(getDevice(), _vulkan_allocator, _transfer_buffer, _renderer.getMaterialDescriptorCache(), _renderer.getNodeDescriptorCache(), _renderer.getMaterialSampler());
        _scene.init();
        // the model.dae file (and diffuse texture) are taken from the ThinMatrix tutorial github:
        // https://github.com/TheThinMatrix/OpenGL-Animation
        loader.importScene("res/model.dae", _scene.addGroup("animation", getDevice(), _vulkan_allocator, _renderer.getNodeDescriptorCache()));
        loader.importScene("res/tex_cube.dae", _scene.addGroup("cube", getDevice(), _vulkan_allocator, _renderer.getNodeDescriptorCache()));
        _transfer_buffer.completeTransfers(_load_cmd_buffer);
        _scene.genMipMaps(_load_cmd_buffer);

        // move and rotate the model to get a better look
        glm::mat4 model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, -10.0f));
        model_mat = glm::rotate(model_mat, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model_mat = glm::rotate(model_mat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        for(SceneGroup& s : _scene.getGroups())
            for(Node& n : s.getRootNode().getChildNodes()) 
                n.setModelMatrix((const uint8_t*)glm::value_ptr(model_mat), _transfer_buffer);

        _load_cmd_buffer.endCommandBuffer();
        getDevice().submitOnGraphicsQueue(_load_cmd_buffer.getCommandBuffer());
        getDevice().waitGraphicsQueueIdle();
    }

    void cleanUp() {

        getDevice().waitForProcessesToFinish();

        _scene.cleanUp();
        _transfer_buffer.cleanUp();
        _load_cmd_buffer.cleanUp();
        _renderer.cleanUp(_swap_chain);
        BasicAppTemplate::cleanUp();
    }

    void framePreperation() {
        // called before the old frame is finished on the gpu

    }

    void transferCommands() {
        // called after the previous frames transfer commands have finished

        // update the camera matrices
        float fov = 90;
        float aspect_ratio = getWindow().getWidth() / float(getWindow().getHeight());
        glm::mat4 cam_view = glm::mat4(1.0f);
        glm::mat4 cam_proj = glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 1000.0f);

        _renderer.loadCameraMatrices(glm::value_ptr(cam_view), glm::value_ptr(cam_proj), _transfer_buffer);

        // record transfer commands
        _transfer_buffer.completeTransfers(getTransferCmd());
    }

    void drawCommands(int swap_image_id) {
        // record draw commands here

        // scene render pass
        _renderer.begin(getDrawCmd(), swap_image_id);
        _renderer.draw(getDrawCmd(), _scene);
        _renderer.end(getDrawCmd());
    }

    void frameFinalization() {
        // called after the previous frame has finished rendering

    }

    void recreateSwapChain() {

        BasicAppTemplate::recreateSwapChain();
        _renderer.recreateFramebuffers(_vulkan_allocator, _swap_chain);
    }

};

int main() {

    AnimationTest app;
    app.init();

    // main loop
    while(app.run());

    app.cleanUp();

    return 0;
}