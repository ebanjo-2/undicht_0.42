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

    _raw_scene.init(device, allocator, node_descriptor_cache);

    // init objects to upload data to the gpu
    TransferBuffer transfer_buffer;
    transfer_buffer.init(allocator, {device.getGraphicsQueueFamily()}, 2000000);
    CommandBuffer transfer_command;
    transfer_command.init(device.getDevice(), device.getGraphicsCmdPool());

    // init scene meshes + textures
    SceneLoader scene_loader;
    scene_loader.importScene("res/sphere.dae", _raw_scene, transfer_buffer, material_descriptor_cache, node_descriptor_cache, sampler, device, allocator);

    // upload data
    transfer_command.beginCommandBuffer(true);
    transfer_buffer.completeTransfers(transfer_command);
    _raw_scene.genMipMaps(transfer_command);
    transfer_command.endCommandBuffer();
    device.submitOnGraphicsQueue(transfer_command.getCommandBuffer());
    device.waitGraphicsQueueIdle();

    // clear nodes created by the scene loader
    _raw_scene.getRootNode().clearChildNodes();

    // cleanup
    transfer_buffer.cleanUp();
    transfer_command.cleanUp();

}

void PhysicsScene::cleanUp() {

    _raw_scene.cleanUp();
}

void PhysicsScene::advanceSimulation(float delta_time) {

    //_simulation.applyUniversalAcceleration(vec3f(0, -9.81, 0), delta_time); // gravity
    _simulation.update(delta_time);

}

void PhysicsScene::clearSimulation() {

    _simulation.clearAllObjects();
}

uint32_t PhysicsScene::addSphere(vec3i position, uint32_t radius, float mass) {

    return _simulation.addSphereObject(SphereObject(position, radius, mass));
}

SphereObject& PhysicsScene::getSphere(uint32_t id) {

    return _simulation.getSphereObject(id);
}

void PhysicsScene::update(const LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, DescriptorSetCache& node_descriptor_cache, TransferBuffer& transfer_buffer) {
    
    for(int i = 0; i < _simulation.getSphereObjects().size(); i++) {

        if(_raw_scene.getRootNode().getChildNodeCount() == i) {
            // adding a new node for the sphere
            _raw_scene.getRootNode().addChildNode(device, allocator, node_descriptor_cache);
            _raw_scene.getRootNode().getChildNode(i).setMeshes({0}); // should be the sphere mesh
        }

        // updating the existing node to match the sphere
        glm::vec3 position = glm::vec3(_simulation.getSphereObject(i).getPosition()) / 10000.0f;
        float radius = _simulation.getSphereObject(i).getRadius() / 10000.0f;
        glm::mat4 model_matrix = glm::translate(position) * _simulation.getSphereObject(i).getRotationMatrix() * glm::scale(radius, radius, radius);
        _raw_scene.getRootNode().getChildNode(i).setModelMatrix((uint8_t*)glm::value_ptr(model_matrix), transfer_buffer);

    }

}

undicht::graphics::Scene& PhysicsScene::getScene() {

    return _raw_scene;
}