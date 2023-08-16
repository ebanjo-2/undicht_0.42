#include "monitor.h"

namespace undicht {

    namespace graphics {

        Monitor::Monitor(GLFWmonitor* monitor) : _monitor(monitor){
            // only the WindowAPI can create a Monitor handle
        }
        
        void Monitor::getSize(uint32_t &width, uint32_t &height) const {
            
            const GLFWvidmode* mode = glfwGetVideoMode(_monitor);
            width = mode->width;
            height = mode->height;
        }

        uint32_t Monitor::getRefreshRate() const {

            return glfwGetVideoMode(_monitor)->refreshRate;
        }

        std::string Monitor::info() const {

            return glfwGetMonitorName(_monitor);
        }

    } // namespace graphics

} // namespace undicht