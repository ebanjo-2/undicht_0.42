#include "application_templates/basic_app_template.h"
#include "3D/camera/free_camera.h"
#include "sky_renderer.h"

using namespace undicht;

class SkyTest : public BasicAppTemplate {
  
  private:

    FreeCamera _cam;
    SkyRenderer _renderer;

  public:

    void init() {

        BasicAppTemplate::init("Sky Test", VK_PRESENT_MODE_FIFO_KHR);
        getWindow().setSize(1000, 800);
        setClearColor({0.01f, 0.0005f, 0.002f, 0.0f});

        _renderer.init(getDevice().getDevice(), _default_render_pass.getRenderPass(), _swap_chain.getExtent());
    }

    void cleanUp() {

        getDevice().waitForProcessesToFinish();

        _renderer.cleanUp();
        BasicAppTemplate::cleanUp();
    }

    void framePreperation() {
        // called before the old frame is finished on the gpu

    }

    void transferCommands() {
        // called after the previous frames transfer commands have finished

    }

    void drawCommands(int swap_image_id) {
        // record draw commands here

        _renderer.begin(getDrawCmd());
        
        _renderer.end(getDrawCmd());
    }

    void frameFinalization() {
        // called after the previous frame has finished rendering

    }

    void recreateSwapChain() {

        BasicAppTemplate::recreateSwapChain();
    }
};

int main() {

    SkyTest app;
    app.init();

    // main loop
    while(app.run() && !app.getWindow().isKeyPressed(GLFW_KEY_ESCAPE));

    app.cleanUp();

    return 0;
}
