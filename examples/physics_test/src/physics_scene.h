#ifndef PHYSICS_SCENE_H
#define PHYSICS_SCENE_H

#include "scene/scene.h"
#include "glm/glm.hpp"

#include "vector"

class PhysicsScene {

  protected:

    undicht::graphics::Scene _graphics_scene;


  public:

    void init(const undicht::vulkan::LogicalDevice& device, undicht::vma::VulkanMemoryAllocator& allocator, undicht::vulkan::DescriptorSetCache& material_descriptor_cache, undicht::vulkan::DescriptorSetCache& node_descriptor_cache, const undicht::vulkan::Sampler& sampler);
    void cleanUp();

    void setupObjects();
    void clearSimulation();

    void update(const undicht::vulkan::LogicalDevice& device, undicht::vma::VulkanMemoryAllocator& allocator, undicht::vulkan::DescriptorSetCache& node_descriptor_cache, undicht::vulkan::TransferBuffer& transfer_buffer);
    
    undicht::graphics::Scene& getScene();

};

#endif // PHYSICS_SCENE