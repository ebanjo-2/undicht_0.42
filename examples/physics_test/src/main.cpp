#include "iostream"
#include "primitives/sphere.h"
#include "debug.h"
#include "random"

#include "physics_scene.h"
#include "physics_renderer.h"
#include "application.h"
#include "vulkan_memory_allocator.h"

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace undicht;
using namespace physics;

void setupPhysicsScene(PhysicsScene& scene) {

    scene.addSphere(vec3i(30000, 30000, 0), 1000, 1.0f);
    scene.addSphere(vec3i(0, -10000, 0), 10000, 1.0f);
    scene.addSphere(vec3i(-20000, 0, 0), 5000, 1.0f);
    scene.addSphere(vec3i(0, -2000000000 - 40000, 0), 2000000000, 1.0f);

    //scene.getSphere(0).setVelocity(vec3f(0, -20, 0));
    //scene.getSphere(1).setVelocity(vec3f(0, 2, 0));
}

int main() {

    Application app;
    app.init("undicht Physics Test", VK_PRESENT_MODE_FIFO_KHR);

    vma::VulkanMemoryAllocator vulkan_allocator;
    vulkan_allocator.init(app.getVulkanInstance().getInstance(), app.getDevice().getDevice(), app.getDevice().getPhysicalDevice());

    PhysicsRenderer renderer;
    renderer.init(app.getDevice(), app.getSwapChain(), vulkan_allocator);

    PhysicsScene scene;
    scene.init(app.getDevice(), vulkan_allocator, renderer.getMaterialDescriptorCache(), renderer.getNodeDescriptorCache(), renderer.getMaterialSampler());
    setupPhysicsScene(scene);

    glm::mat4 camera_view = glm::lookAt(glm::vec3(0.0f, 0.0, 5.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
    glm::mat4 camera_proj = glm::perspective(90.0f, float(app.getWindow().getWidth()) / app.getWindow().getHeight(), 0.1f, 1000.0f);   

    long last_micros = micros();

    while(!app.getWindow().shouldClose()) {

        float delta_time = (micros() - last_micros) / 1000000.0;
        last_micros = micros();

        // user input
        if(app.getWindow().isKeyPressed(GLFW_KEY_R)) {
            // reset the scene
            scene.clearSimulation();
            setupPhysicsScene(scene);
        }

        // physics!
        scene.getSphere(0).addVelocity(vec3f(0.0f, -9.81f, 0.0f) * delta_time);
        scene.getSphere(1).addVelocity(vec3f(0.0f, -9.81f, 0.0f) * delta_time);
        scene.getSphere(2).addVelocity(vec3f(0.0f, -9.81f, 0.0f) * delta_time);


        scene.advanceSimulation(delta_time);
        //scene.advanceSimulation(0.0166);

        // draw the scene
        if(renderer.newFrame(app.getSwapChain())) {
            
            scene.update(app.getDevice(), vulkan_allocator, renderer.getNodeDescriptorCache(), renderer.getTransferBuffer());
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
    renderer.cleanUp(app.getSwapChain());
    vulkan_allocator.cleanUp();
    app.cleanUp();

}