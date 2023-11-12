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

    scene.addSphere(vec3i(0, -2000000 - 40000, 0), 2000000, 10000000.0f, 0.8f); // seams to be close to max size 

    scene.addSphere(vec3i(0, 63000, 0), 5000, 10.0f, 0.1f);
    /*scene.addSphere(vec3i(0, 56000, 0), 1000, 1.0f, 0.1f);
    scene.addSphere(vec3i(0, 54000, 0), 1000, 1.0f, 0.1f);
    scene.addSphere(vec3i(0, 52000, 0), 1000, 1.0f, 0.1f);*/
    scene.addSphere(vec3i(0, 50000, 0), 1000, 1.1f, 0.1f);
    scene.addSphere(vec3i(0, 40000, 0), 3000, 400.0f, 0.8f);
    scene.addSphere(vec3i(0, 30000, 0), 5000, 10000.0f, 0.6f);
    scene.addSphere(vec3i(0, -10000, 0), 10000, 100000.0f, 0.8f);
    scene.addSphere(vec3i(-20000, 0, 0), 2000, 100.0f, 0.8f);

    //scene.getSphere(2).setVelocity(vec3f(0, -200, 0));
    //scene.getSphere(1).setVelocity(vec3f(0, 2, 0));
}

int main() {

    Application app;
    //app.init("undicht Physics Test", VK_PRESENT_MODE_IMMEDIATE_KHR);
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
        // advance the physics "simulation"
        scene.advanceSimulation(glm::min(delta_time, 0.0166f));
        //scene.advanceSimulation(0.0166);

        // hold the big sphere in place
        scene.getSphere(0).setPosition(vec3i(0, -2000000 - 40000, 0)); 
        scene.getSphere(0).setVelocity(vec3f(0.0f)); 

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