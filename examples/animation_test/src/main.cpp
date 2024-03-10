#include "application_templates/basic_app_template.h"
#include "renderer/vulkan/transfer_buffer.h"
#include "scene/renderer/scene_renderer.h"
#include "scene/scene.h"
#include "scene_loader/scene_loader.h"
#include "3D/camera/free_camera.h"

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

    FreeCamera _cam;

  public:

    void init() {

        BasicAppTemplate::init("Animation Test", VK_PRESENT_MODE_FIFO_KHR);
        getWindow().setSize(1000, 800);
        setClearColor({0.01f, 0.0005f, 0.002f, 0.0f});

        _load_cmd_buffer.init(getDevice().getDevice(), getDevice().getGraphicsCmdPool());
        _load_cmd_buffer.beginCommandBuffer(true);

        _transfer_buffer.init(_vulkan_allocator, {getDevice().getGraphicsQueueFamily()}, 10000000); // reserve 10 Mb
        _renderer.init(getDevice(), getSwapChain(), _vulkan_allocator);

        SceneLoader loader;
        loader.setInitObjects(getDevice(), _vulkan_allocator, _transfer_buffer, _renderer.getMaterialDescriptorCache(), _renderer.getNodeDescriptorCache(), _renderer.getMaterialSampler());
        _scene.init();
        // the model.dae file (and diffuse texture) are taken from the ThinMatrix tutorial github:
        // https://github.com/TheThinMatrix/OpenGL-Animation
        loader.importScene("res/model.dae", _scene.addGroup("animation"));
        loader.importScene("res/bob/boblampclean.md5mesh", _scene.addGroup("bob_lamp"));
        loader.importScene("res/tex_cube.dae", _scene.addGroup("cube"));
        _transfer_buffer.completeTransfers(_load_cmd_buffer);
        _transfer_buffer.reset();
        _scene.genMipMaps(_load_cmd_buffer);


        // move and rotate the model to get a better look
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        _scene.getGroup("animation")->getRootNode().setLocalTransformation(model);

        _transfer_buffer.completeTransfers(_load_cmd_buffer);
        _transfer_buffer.reset();
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

        // show / hide the cursor
        if(getWindow().isKeyPressed(GLFW_KEY_LEFT_ALT))
            getWindow().setCursorEnabled(false);
        else
            getWindow().setCursorEnabled(true);

        // animations
        //Bone* bone = _scene.getGroup("animation")->getBone("Head");
        //bone->setLocalMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0,-1,0)));
        _scene.updateAnimations(glfwGetTime());

        // update bone matrices
        _scene.updateBoneMatrices();
        _scene.updateGlobalTransformations();

    }

    void transferCommands() {
        // called after the previous frames transfer commands have finished

        _transfer_buffer.reset();

        // update the node ubos
        _scene.updateNodeUBOs(_transfer_buffer);

        // update the camera matrices
        _cam.update(getWindow());
        glm::mat4 cam_view = _cam.getView();
        glm::mat4 cam_proj = _cam.getProjection(100.0f, float(getWindow().getWidth()) / getWindow().getHeight());

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
    while(app.run() && !app.getWindow().isKeyPressed(GLFW_KEY_ESCAPE));

    app.cleanUp();

    return 0;
}