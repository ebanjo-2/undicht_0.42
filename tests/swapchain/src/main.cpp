#include "iostream"
#include "vector"
#include "cassert"

#include "debug.h"
#include "engine.h"

#include "window/glfw/window.h"
#include "core/vulkan/logical_device.h"
#include "core/vulkan/swap_chain.h"

using namespace undicht;
using namespace graphics;
using namespace vulkan;

int main() {

    // initializing the engine and opening a window
    Engine undicht_engine;
    undicht_engine.init();
    Window glfw_window;
    glfw_window.open(undicht_engine.getVulkanInstance().getInstance(), "Swapchain Test");
    LogicalDevice device;
    device.init(undicht_engine.getVulkanInstance().chooseGPU(), glfw_window.getSurface());

    UND_LOG << "testing swapchain creation (Gpu: " << device.info() << ")\n";

    // creating the swap chain
    SwapChain swap_chain;
    swap_chain.init(device, glfw_window.getSurface());

    UND_LOG << "initialized the swap chain, swap images: " << swap_chain.getSwapImageCount() << "\n";

    // retrieving swap images
    std::vector<ImageView> swap_images = swap_chain.retrieveSwapImages();
    UND_LOG << "retrieved swap images: " << swap_images.size() << "\n";

    assert(swap_images.size() == swap_chain.getSwapImageCount());

    // resize the window & recreate the swap chain
    glfw_window.setSize(1000, 800);
    swap_chain.recreate(device, glfw_window.getSurface());
    swap_chain.freeSwapImages(swap_images);
    swap_images = swap_chain.retrieveSwapImages();

    UND_LOG << "resized the window and recreated the swap chain\n";

    // cleanup
    swap_chain.freeSwapImages(swap_images);
    swap_chain.cleanUp();
    device.cleanUp();
    glfw_window.close();
    undicht_engine.cleanUp();

    UND_LOG << "Test complete! \n";

    return 0;
}