#include "window.h"
#include "debug.h"
#include "core/vulkan/vk_debug.h"

namespace undicht {

    namespace graphics {

        Window::Window(const VkInstance& instance, const std::string &title, uint32_t width, uint32_t height) {

            open(instance, title, width, height);

        }

        void Window::open(const VkInstance& instance, const std::string &title, uint32_t width, uint32_t height) {

            // storing the vulkan instance that the graphics surface belongs to
            _instance = instance;

            // only for opengl 3.3
            // glfwWindowHint(GLFW_VERSION_MAJOR, 3);
            // glfwWindowHint(GLFW_VERSION_MINOR, 3);
            // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            // only for vulkan
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // temporary. will need some work later

            // creating the window
            _width = width;
            _height = height;
            _has_resized = true;
            _title = title;
            _window = glfwCreateWindow(width, height, title.data(), 0, 0);

            if (_window == NULL) {
                UND_ERROR << "failed to open window\n";
                return;
            }

            // creating the vulkan graphics surface
            VK_ASSERT(glfwCreateWindowSurface(instance, _window, nullptr, &_surface));

        }

        void Window::close() {

            glfwDestroyWindow(_window);
            vkDestroySurfaceKHR(_instance, _surface, {});
        }

        void Window::setTitle(const std::string &title) {
            
            _title = title;
            glfwSetWindowTitle(_window, title.data());
        }

        std::string Window::getTitle() const {

            return _title;
        }

        void Window::setSize(uint32_t width, uint32_t height) {

            if((width == _width) && (_height == height))
                return;

            _width = width;
            _height = height;
            _has_resized = true;

            glfwSetWindowSize(_window, width, height);
        }

        void Window::getSize(uint32_t &width, uint32_t &height) const {

            width = _width;
            height = _height;
        }

        uint32_t Window::getWidth() const {

            return _width;
        }

        uint32_t Window::getHeight() const {

            return _height;
        }

        void Window::setFullscreen(Monitor* monitor) {

            _fullscreen = true;

            uint32_t width, height, refresh_rate;
            monitor->getSize(width, height);
            refresh_rate = monitor->getRefreshRate();

            glfwSetWindowMonitor(_window, monitor->_monitor, 0, 0, width, height, refresh_rate);
        }

        void Window::setWindowed(uint32_t &width, uint32_t &height) {
            // undo fullscreen

            _fullscreen = false;

            glfwSetWindowMonitor(_window, 0, 0, 0, width, height, 0);
        } 

        bool Window::isFullscreen() const {

            return _fullscreen;
        }

        void Window::setCursorEnabled(bool enabled) {

            if(enabled)
                glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            else
                glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        }

        void Window::update() {

            glfwPollEvents();

            // checking if the window has resized
            int new_width, new_height;
            glfwGetFramebufferSize(_window, &new_width, &new_height);

            if((_width != new_width) || (_height != new_height)) {

                _width = new_width;
                _height = new_height;
                _has_resized = true;
            } else {

                _has_resized = false;
            }

        }

        void Window::waitForEvent() {

            glfwWaitEvents();
        }

        ////////////////////////////////////////// events /////////////////////////////////////

        bool Window::shouldClose() const{

            return glfwWindowShouldClose(_window);
        }

        bool Window::hasResized() const {

            return _has_resized;
        }

        bool Window::isMinimized() const {

            return glfwGetWindowAttrib(_window, GLFW_ICONIFIED);
        }

        ///////////////////////////////////// user input /////////////////////////////////////

        bool Window::isKeyPressed(int key_id) const {

            return glfwGetKey(_window, key_id);
        }

        bool Window::isMouseButtonPressed(int button_id) const {

            return glfwGetMouseButton(_window, button_id);
        }

        void Window::getCursorPos(double& x, double& y) const {

            glfwGetCursorPos(_window, &x, &y);
        }


        const VkSurfaceKHR& Window::getSurface() const {

            return _surface;
        }

        const GLFWwindow* Window::getWindow() const {

            return _window;
        }


    } // namespace graphics

} // namespace undicht