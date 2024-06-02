#include "free_camera.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/exponential.hpp"

#include "debug.h"

using namespace undicht;

void FreeCamera::update(const undicht::graphics::Window& window) {

    ///// translation /////

    glm::vec3 forward(glm::sin(_yaw) * glm::cos(_pitch), glm::sin(_pitch), glm::cos(_yaw) * glm::cos(_pitch));
    forward = glm::normalize(forward);
    //glm::vec3 forward = glm::vec3(0,0,-1);


    glm::vec3 left = glm::cross(glm::vec3(0,1,0), forward);
    left = glm::normalize(left);

    if(window.isKeyPressed(GLFW_KEY_W)) _position += forward * 0.2f;
    if(window.isKeyPressed(GLFW_KEY_S)) _position -= forward * 0.2f;
    if(window.isKeyPressed(GLFW_KEY_A)) _position += left * 0.2f;
    if(window.isKeyPressed(GLFW_KEY_D)) _position -= left * 0.2f;

    ///// rotation /////

    if(window.isCursorEnabled()) return; // dont want to rotate the camera in this case

    double cursor_x, cursor_y;
    window.getCursorPos(cursor_x, cursor_y);

    _yaw -= (cursor_x - _last_cursor_x) * 0.003;
    _pitch -= (cursor_y - _last_cursor_y) * 0.003;

    _last_cursor_x = cursor_x;
    _last_cursor_y = cursor_y;
}

glm::mat4 FreeCamera::getProjection(float fov, float aspect_ratio) {
    // vulkan uses a different ndc (normalized device coordinate system) to opengl
    // the depth values only go from 0 to 1 and not from -1 to 1,
    // the -y axis is up and +z is forward
    // this is why the standard glm functions for creating a projection matrix 
    // for mapping to the ndc doesnt just work
    // using #define GLM_FORCE_DEPTH_ZERO_TO_ONE and multiplying [1][1] by -1 fixes this
    // according to here: https://computergraphics.stackexchange.com/questions/12448/vulkan-perspective-matrix-vs-opengl-perspective-matrix

    glm::mat4 proj = glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 1000.0f);
    proj[1][1] *= -1;

    return proj;
}

glm::mat4 FreeCamera::getView() {

    glm::vec3 forward(glm::sin(_yaw) * glm::cos(_pitch), glm::sin(_pitch), glm::cos(_yaw) * glm::cos(_pitch));
    forward = glm::normalize(forward);

    // UND_LOG << "x:" << _position.x << " y:" << _position.y << " z:" << _position.z << "\n";

    // setting +y to be the up direction
    return  glm::lookAt(_position, _position + forward, glm::vec3(0.0f , 1.0f , 0.0f));
}