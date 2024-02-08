#include "free_camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/exponential.hpp"

void FreeCamera::update(const undicht::graphics::Window& window) {

    ///// translation /////

    glm::vec3 forward(glm::sin(_yaw) * glm::cos(_pitch), glm::sin(_pitch), glm::cos(_yaw) * glm::cos(_pitch));
    forward = glm::normalize(forward);
    //glm::vec3 forward = glm::vec3(0,0,-1);


    glm::vec3 left = glm::cross(forward, glm::vec3(0,1,0));
    left = glm::normalize(left);

    if(window.isKeyPressed(GLFW_KEY_W)) _position += forward * 0.2f;
    if(window.isKeyPressed(GLFW_KEY_S)) _position -= forward * 0.2f;
    if(window.isKeyPressed(GLFW_KEY_A)) _position += left * 0.2f;
    if(window.isKeyPressed(GLFW_KEY_D)) _position -= left * 0.2f;

    ///// rotation /////

    if(window.isCursorEnabled()) return; // dont want to rotate the camera in this case

    double cursor_x, cursor_y;
    window.getCursorPos(cursor_x, cursor_y);

    _yaw += (cursor_x - _last_cursor_x) * 0.003;
    _pitch += (cursor_y - _last_cursor_y) * 0.003;

    _last_cursor_x = cursor_x;
    _last_cursor_y = cursor_y;
}

glm::mat4 FreeCamera::getProjection(float fov, float aspect_ratio) {
    
    return glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 1000.0f);
}

glm::mat4 FreeCamera::getView() {

    glm::vec3 forward(glm::sin(_yaw) * glm::cos(_pitch), glm::sin(_pitch), glm::cos(_yaw) * glm::cos(_pitch));
    // glm::vec3 forward(glm::sin(_yaw) * glm::cos(_pitch), glm::sin(_pitch), glm::cos(_yaw) * glm::cos(_pitch));
    forward = glm::normalize(forward);
    //glm::vec3 forward = glm::vec3(0,0,-1);

    return  glm::lookAt(_position, _position + forward, glm::vec3(0,-1,0));
}