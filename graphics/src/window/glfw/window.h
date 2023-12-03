#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "string"

#include "monitor.h"

namespace undicht {

    namespace graphics {

        class Window {
            
            private:

            GLFWwindow* _window = 0;
            VkSurfaceKHR _surface;
            VkInstance _instance;

            bool _fullscreen = false;
            uint32_t _width = 0;
            uint32_t _height = 0;
            bool _has_resized = false; // since the last frame
            std::string _title;

            public:

            Window() = default;
            Window(const VkInstance& instance, const std::string& title = "", uint32_t width = 800, uint32_t height = 600);

            void open(const VkInstance& instance, const std::string& title = "", uint32_t width = 800, uint32_t height = 600);
            void close();

            void setTitle(const std::string& title);
            std::string getTitle() const;

            void setSize(uint32_t width, uint32_t height);
            void getSize(uint32_t &width, uint32_t &height) const;
            uint32_t getWidth() const;
            uint32_t getHeight() const;

            void setFullscreen(const Monitor& monitor);
            void setWindowed(uint32_t width, uint32_t height); // undo fullscreen
            bool isFullscreen() const;

            void setCursorEnabled(bool enabled);
            bool isCursorEnabled() const;

            void update();
            void waitForEvent(); // waits for events such as user input, which might prompt the contents of the window to get updated

            // events
            bool shouldClose() const;
            bool hasResized() const;
            bool isMinimized() const;

            // user input
            bool isKeyPressed(int key_id) const;
            bool isMouseButtonPressed(int button_id) const;
            void getCursorPos(double& x, double& y) const;

            const VkSurfaceKHR& getSurface() const;
            const GLFWwindow* getWindow() const;

        };

    } // namespace graphics

} // namespace undicht

#endif // WINDOW_H
