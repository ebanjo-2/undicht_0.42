#ifndef FREE_CAMERA_H
#define FREE_CAMERA_H

#include "glm/glm.hpp"
#include "window/glfw/window.h"

class FreeCamera {
  
  protected:

    glm::vec3 _position = glm::vec3(0.0f);
    float _yaw = 0.0f;
    float _pitch = 0.0f;

    double _last_cursor_x = 0;
    double _last_cursor_y = 0;

  public:

    void update(const undicht::graphics::Window& window);

    glm::mat4 getProjection(float fov, float aspect_ratio);
    glm::mat4 getTransformation();

};

#endif // FREE_CAMERA_H