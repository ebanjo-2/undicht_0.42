#include "application_templates/basic_app_template.h"

using namespace undicht;
using namespace graphics;
using namespace vulkan;

class AnimationTest : public BasicAppTemplate {

  public:

    void init() {

        BasicAppTemplate::init("Animation Test", VK_PRESENT_MODE_FIFO_KHR);
        getWindow().setSize(1000, 800);
        //setClearColor({0.01f, 0.0005f, 0.002f, 0.0f});

    }

    void cleanUp() {

        getDevice().waitForProcessesToFinish();

        BasicAppTemplate::cleanUp();
    }

    void framePreperation() {
        // called before the old frame is finished on the gpu

    }

    void transferCommands() {
        // called after the previous frames transfer commands have finished

    }

    void drawCommands() {
        // record draw commands here

    }

    void frameFinalization() {
        // called after the previous frame has finished rendering

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