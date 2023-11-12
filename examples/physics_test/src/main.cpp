#include "iostream"

#include "application.h"
#include "vulkan_memory_allocator.h"

#include "physics_renderer.h"
#include "physics_scene.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "physics.h"


using namespace undicht;


int main() {

    Application app;
    //app.init("undicht Physics Test", VK_PRESENT_MODE_IMMEDIATE_KHR);
    app.init("undicht Physics Test", VK_PRESENT_MODE_FIFO_KHR);

    vma::VulkanMemoryAllocator vulkan_allocator;
    vulkan_allocator.init(app.getVulkanInstance().getInstance(), app.getDevice().getDevice(), app.getDevice().getPhysicalDevice());

    PhysicsRenderer renderer;
    renderer.init(app.getDevice(), app.getSwapChain(), vulkan_allocator);

    physics::initJoltPhysics();

    PhysicsScene scene;
    scene.init(app.getDevice(), vulkan_allocator, renderer.getMaterialDescriptorCache(), renderer.getNodeDescriptorCache(), renderer.getMaterialSampler());

    glm::mat4 camera_view = glm::lookAt(glm::vec3(0.0f, 0.0, 5.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
    glm::mat4 camera_proj = glm::perspective(90.0f, float(app.getWindow().getWidth()) / app.getWindow().getHeight(), 0.1f, 1000.0f);   

    while(!app.getWindow().shouldClose()) {

        if(!app.getWindow().isKeyPressed(GLFW_KEY_P))
            scene.updatePhysics();

        // draw the scene
        if(renderer.newFrame(app.getSwapChain())) {

            scene.updateGraphics(app.getDevice(), vulkan_allocator, renderer.getNodeDescriptorCache(), renderer.getTransferBuffer());
            
            renderer.loadCameraMatrices(camera_view, camera_proj);
            renderer.drawScene(scene);
            renderer.endFrame(app.getDevice(), app.getSwapChain());

        } else {
            
            app.recreateSwapChain(VK_PRESENT_MODE_FIFO_KHR);
            renderer.recreateSwapChain(app.getSwapChain(), vulkan_allocator);
            camera_proj = glm::perspective(90.0f, float(app.getWindow().getWidth()) / app.getWindow().getHeight(), 0.1f, 1000.0f);
        }

        app.getWindow().update();
    }

    app.getDevice().waitForProcessesToFinish();
    scene.cleanUp();
    physics::cleanUpJoltPhysics();
    renderer.cleanUp(app.getSwapChain());
    vulkan_allocator.cleanUp();
    app.cleanUp();

}