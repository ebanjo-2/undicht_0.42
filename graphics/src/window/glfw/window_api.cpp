#include "window_api.h"

#include "GLFW/glfw3.h"

namespace undicht {

    namespace graphics {

        void WindowAPI::init() {

            glfwInit();
        }

        void WindowAPI::cleanUp() {

            glfwTerminate();
        }

        uint32_t WindowAPI::getMonitorCount() const {

            int count;
            glfwGetMonitors(&count);

            return count;
        }

        Monitor WindowAPI::getMonitor(uint32_t id) const{

            GLFWmonitor** monitors;
            int count;
            monitors = glfwGetMonitors(&count);

            if(id < count) {

                return Monitor(monitors[id]);
            } else {

                return Monitor(0);
            }

        }

        std::string WindowAPI::info() const {

            return "GLFW V3.1";
        }

    } // namespace graphics

} // namespace undicht
