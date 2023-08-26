#include "engine.h"
#include "debug.h"

namespace undicht {

    void Engine::init() {
        /** @brief initializes core engine components 
         * initializes glfw
         * creates a vulkan instance 
        */

        _window_api.init(); // needs to be initialized before the instance, because the instance gets the names of the required extensions from glfw
        _vk_instance.init();

    }

    void Engine::cleanUp() {
        /** clean up the core engine objects */
        // in the opposite order in which they were created

        // terminating the vulkan instance
        _vk_instance.cleanUp();

        // terminating glfw
        _window_api.cleanUp();
    }

    const vulkan::Instance& Engine::getVulkanInstance() const {

        return _vk_instance;
    }

    const graphics::WindowAPI& Engine::getWindowAPI() const {

        return _window_api;
    }

} // namespace undicht