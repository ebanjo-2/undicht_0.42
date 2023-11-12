#include "physics_scene.h"
#include "scene_loader/texture_loader.h"
#include "scene_loader/scene_loader.h"
#include "renderer/vulkan/transfer_buffer.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "debug.h"

#include <Jolt/Jolt.h>
#include "Jolt/Geometry/Sphere.h"

using namespace undicht;
using namespace graphics;
using namespace vulkan;
using namespace tools;

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

void PhysicsScene::setupObjects() {



}

void PhysicsScene::clearSimulation() {

}

void PhysicsScene::update(const LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, DescriptorSetCache& node_descriptor_cache, TransferBuffer& transfer_buffer) {
    
}

undicht::graphics::Scene& PhysicsScene::getScene() {

    return _graphics_scene;
}