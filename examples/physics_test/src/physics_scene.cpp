#include "physics_scene.h"
#include "scene_loader/texture_loader.h"
#include "scene_loader/scene_loader.h"
#include "renderer/vulkan/transfer_buffer.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "debug.h"
#include "math/physics_types.h"

using namespace undicht;
using namespace graphics;
using namespace vulkan;
using namespace tools;
using namespace physics;

void PhysicsScene::init(const LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, DescriptorSetCache& material_descriptor_cache, DescriptorSetCache& node_descriptor_cache, const Sampler& sampler) {

    _graphics_scene.init(device, allocator, node_descriptor_cache);

    // init objects to upload data to the gpu
    TransferBuffer transfer_buffer;
    transfer_buffer.init(allocator, {device.getGraphicsQueueFamily()}, 2000000);
    CommandBuffer transfer_command;
    transfer_command.init(device.getDevice(), device.getGraphicsCmdPool());

    // init scene meshes + textures
    SceneLoader scene_loader;
    scene_loader.importScene("res/sphere.dae", _graphics_scene, transfer_buffer, material_descriptor_cache, node_descriptor_cache, sampler, device, allocator);

    // upload data
    transfer_command.beginCommandBuffer(true);
    transfer_buffer.completeTransfers(transfer_command);
    _graphics_scene.genMipMaps(transfer_command);
    transfer_command.endCommandBuffer();
    device.submitOnGraphicsQueue(transfer_command.getCommandBuffer());
    device.waitGraphicsQueueIdle();

    // clear nodes created by the scene loader
    _graphics_scene.getRootNode().clearChildNodes();

    // cleanup
    transfer_buffer.cleanUp();
    transfer_command.cleanUp();

}

void PhysicsScene::cleanUp() {

    _graphics_scene.cleanUp();
}

void PhysicsScene::advanceSimulation(float delta_time) {

    /*_simulation.addObjectAcceleration(&_spheres.at(0), vec3f(0, -9.81, 0)); // gravity*/
    //_simulation.addObjectAcceleration(&_spheres.at(1), vec3f(0, -9.81, 0)); // gravity
    /*_simulation.addObjectAcceleration(&_spheres.at(2), vec3f(0, -9.81, 0)); // gravity
    _simulation.addObjectAcceleration(&_spheres.at(3), vec3f(0, -9.81, 0)); // gravity
    _simulation.addObjectAcceleration(&_spheres.at(4), vec3f(0, -9.81, 0)); // gravity
    _simulation.addObjectAcceleration(&_spheres.at(5), vec3f(0, -9.81, 0)); // gravity
    _simulation.addObjectAcceleration(&_spheres.at(6), vec3f(0, -9.81, 0)); // gravity
    _simulation.addObjectAcceleration(&_spheres.at(7), vec3f(0, -9.81, 0)); // gravity
    _simulation.addObjectAcceleration(&_spheres.at(8), vec3f(0, -9.81, 0)); // gravity*/
    _simulation.addUniversalAcceleration(vec3f(0, -9.81, 0)); // gravity
    _simulation.addObjectAcceleration(&_spheres.at(0), vec3f(0, 9.81, 0)); // anti-gravity for the big sphere
    _simulation.update(delta_time, _spheres); // resets all accelerations

}

void PhysicsScene::clearSimulation() {

    _spheres.clear();
}

uint32_t PhysicsScene::addSphere(vec3i position, uint32_t radius, float mass, float bounce_coeff) {

    _spheres.push_back(SphereObject(position, radius, mass, bounce_coeff));

    return _spheres.size() - 1;
}

SphereObject& PhysicsScene::getSphere(uint32_t id) {

    return _spheres.at(id);
}

uint32_t PhysicsScene::addCuboid(vec3i position, const vec3i& half_size, float mass, float bounce_coeff) {

    _cuboids.push_back(CuboidObject(position, half_size, mass, bounce_coeff));

    return _cuboids.size() - 1;
}

CuboidObject& PhysicsScene::getCuboid(uint32_t id) {

    return _cuboids.at(id);
}

void PhysicsScene::update(const LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, DescriptorSetCache& node_descriptor_cache, TransferBuffer& transfer_buffer) {
    
    for(int i = 0; i < _spheres.size(); i++) {

        if(_graphics_scene.getRootNode().getChildNodeCount() == i) {
            // adding a new node for the sphere
            _graphics_scene.getRootNode().addChildNode(device, allocator, node_descriptor_cache);
        }

        // updating the existing node to match the sphere
        _graphics_scene.getRootNode().getChildNode(i).setMeshes({0}); // should be the sphere mesh
        glm::vec3 position = glm::vec3(_spheres.at(i).getPosition()) / 10000.0f;
        float radius = _spheres.at(i).getRadius() / 10000.0f;
        glm::mat4 model_matrix = glm::translate(position) * _spheres.at(i).getRotationMatrix() * glm::scale(radius, radius, radius);
        _graphics_scene.getRootNode().getChildNode(i).setModelMatrix((uint8_t*)glm::value_ptr(model_matrix), transfer_buffer);

    }

}

undicht::graphics::Scene& PhysicsScene::getScene() {

    return _graphics_scene;
}