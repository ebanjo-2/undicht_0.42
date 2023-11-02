#ifndef PHYSICS_SCENE_H
#define PHYSICS_SCENE_H

#include "scene/scene.h"
#include "glm/glm.hpp"

#include "objects/sphere_object.h"
#include "simulation/physics_simulation.h"
#include "math/physics_types.h"
#include "vector"

class PhysicsScene {

  protected:

	std::vector<undicht::physics::SphereObject> _spheres;

    undicht::graphics::Scene _graphics_scene;
    undicht::physics::PhysicsSimulation _simulation;

  public:

    void init(const undicht::vulkan::LogicalDevice& device, undicht::vma::VulkanMemoryAllocator& allocator, undicht::vulkan::DescriptorSetCache& material_descriptor_cache, undicht::vulkan::DescriptorSetCache& node_descriptor_cache, const undicht::vulkan::Sampler& sampler);
    void cleanUp();

    void advanceSimulation(float delta_time);
    
    void clearSimulation();

    uint32_t addSphere(undicht::physics::vec3i position, uint32_t radius, float mass);
    undicht::physics::SphereObject& getSphere(uint32_t id);
    void update(const undicht::vulkan::LogicalDevice& device, undicht::vma::VulkanMemoryAllocator& allocator, undicht::vulkan::DescriptorSetCache& node_descriptor_cache, undicht::vulkan::TransferBuffer& transfer_buffer);
    
    undicht::graphics::Scene& getScene();

};

#endif // PHYSICS_SCENE