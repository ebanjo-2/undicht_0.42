#include "iostream"
#include <cassert>

#include "debug.h"
#include <core/vulkan/instance.h>
#include "core/vulkan/logical_device.h"

#include "window/glfw/window_api.h"
#include "window/glfw/monitor.h"
#include "window/glfw/window.h"

using namespace undicht;
using namespace vulkan;
using namespace graphics;

int main() {

    // creating a vulkan instance
    // and choosing a gpu to use

    UND_LOG << "Testing vulkan initialization\n";

    // initializing glfw (has to be done before initializing vulkan, since the vulkan instance need glfw to tell it which extensions to use)
    WindowAPI window_api;
    window_api.init();
    UND_LOG << "Initialized GLFW\n";

    // creating the vulkan instance
    Instance vk_instance;
    vk_instance.init();
    UND_LOG << "Initialized Vulkan Instance\n";

    // opening a glfw window (which creates a vulkan surface)
    Window glfw_window(vk_instance.getInstance());
    UND_LOG << "Opened a window using glfw\n";

    // initializing a vulkan logical device
    LogicalDevice vk_device;
    vk_device.init(vk_instance.chooseGPU(), glfw_window.getSurface());
    UND_LOG << "Initialized Vulkan Logical Device: " << vk_device.info() << "\n";

    // cleanup (in reverse order of initialization)
    vk_device.cleanUp();
    glfw_window.close();
    vk_instance.cleanUp();
    window_api.cleanUp();
    UND_LOG << "Cleanup Successful\n";

    UND_LOG << "All Tests for vulkan initialization passed!\n";
    return 0;
}