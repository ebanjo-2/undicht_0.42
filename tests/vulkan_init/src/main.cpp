#include "iostream"
#include <cassert>

#include "debug.h"
#include <core/vulkan/instance.h>

using namespace undicht;
using namespace vulkan;

int main() {

    // creating a vulkan instance, a window (to get a VkSurface object)
    // and choosing a gpu to use

    UND_LOG << "Testing vulkan initialization!\n";

    Instance vk_instance;
    vk_instance.init();
    UND_LOG << "Initialized Vulkan Instance!\n";

    vk_instance.cleanUp();
    UND_LOG << "Destroyed Vulkan Instance!\n";

    UND_LOG << "All Tests for vulkan initialization passed!\n";
    return 0;
}