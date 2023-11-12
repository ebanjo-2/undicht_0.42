#ifndef PHYSICS_SCENE_H
#define PHYSICS_SCENE_H

#include "scene/scene.h"
#include "glm/glm.hpp"

#include "vector"

#include "physics.h"
#include "object_layers.h"
#include "debug_contact_listener.h"
#include "debug_body_activation_listener.h"

#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>


class PhysicsScene {

  protected:

    undicht::graphics::Scene _graphics_scene;

    JPH::TempAllocatorImpl _physics_allocator;
	  JPH::JobSystemThreadPool _physics_job_system;
    undicht::physics::BPLayerInterfaceImpl _broad_phase_layer_interface;
	  undicht::physics::ObjectVsBroadPhaseLayerFilterImpl _object_vs_broadphase_layer_filter;
	  undicht::physics::ObjectLayerPairFilterImpl _object_vs_object_layer_filter;
    JPH::PhysicsSystem _physics_system;
    JPH::BodyInterface* _body_interface = nullptr;
	  undicht::physics::DebugContactListener _contact_listener;
    undicht::physics::DebugBodyActivationListener _activation_listener;

    // physics objects
    //std::vector<JPH::BodyCreationSettings> _body_settings;
    std::vector<JPH::BodyID> _body_ids;

  public:

    PhysicsScene();

    void init(const undicht::vulkan::LogicalDevice& device, undicht::vma::VulkanMemoryAllocator& allocator, undicht::vulkan::DescriptorSetCache& material_descriptor_cache, undicht::vulkan::DescriptorSetCache& node_descriptor_cache, const undicht::vulkan::Sampler& sampler);
    void cleanUp();
    
    void updatePhysics();
    void updateGraphics(const undicht::vulkan::LogicalDevice& device, undicht::vma::VulkanMemoryAllocator& allocator, undicht::vulkan::DescriptorSetCache& node_descriptor_cache, undicht::vulkan::TransferBuffer& transfer_buffer);

    undicht::graphics::Scene& getScene();

    const JPH::BodyID& addSphere(const glm::vec3& position, float radius, bool movable = true);
    const JPH::BodyID& addBox(const glm::vec3& position, const glm::vec3& half_size, bool movable = true);

  protected:
    // internal functions

    void initGraphics(const undicht::vulkan::LogicalDevice& device, undicht::vma::VulkanMemoryAllocator& allocator, undicht::vulkan::DescriptorSetCache& material_descriptor_cache, undicht::vulkan::DescriptorSetCache& node_descriptor_cache, const undicht::vulkan::Sampler& sampler);
    void initPhysics();


};

#endif // PHYSICS_SCENE