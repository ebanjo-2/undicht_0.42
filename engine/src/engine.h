#ifndef UNDICHT_ENGINE_H
#define UNDICHT_ENGINE_H

#include "vector"
#include "chrono"

#include "window/glfw/window_api.h"
#include "window/glfw/monitor.h"

#include "core/vulkan/instance.h"
#include "core/vulkan/logical_device.h"


namespace undicht {

    class Engine {
        // initializes vulkan and the window library
    protected:

        graphics::WindowAPI _window_api;
        vulkan::Instance _vk_instance;

    public:

        /** @brief initializes core engine components 
         * initializes glfw
         * creates a vulkan instance */
        virtual void init();

        /** clean up the core engine components */
        virtual void cleanUp();

        const vulkan::Instance& getVulkanInstance() const;
        const graphics::WindowAPI& getWindowAPI() const;
    };

}

#endif // UNDICHT_ENGINE_H