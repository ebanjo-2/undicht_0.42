#ifndef PHYSICS_RENDERER_H
#define PHYSICS_RENDERER_H

#include "scene/renderer/scene_renderer.h"
#include "frame_manager.h"
#include "physics_scene.h"
#include "renderer/vulkan/transfer_buffer.h"
#include "glm/glm.hpp"

class PhysicsRenderer {
  
  protected:

    undicht::graphics::SceneRenderer _renderer;
    undicht::FrameManager _frame_manager;
    undicht::vulkan::TransferBuffer _transfer_buffer;

    uint32_t _swap_image;

  public:

    void init(const undicht::vulkan::LogicalDevice& device, undicht::vulkan::SwapChain& swap_chain, undicht::vma::VulkanMemoryAllocator& allocator);
    void cleanUp(undicht::vulkan::SwapChain& swap_chain);

    undicht::vulkan::DescriptorSetCache& getMaterialDescriptorCache();
    undicht::vulkan::DescriptorSetCache& getNodeDescriptorCache();
    undicht::vulkan::Sampler& getMaterialSampler();

    void loadCameraMatrices(glm::mat4& view, glm::mat4& proj);
    void drawScene(PhysicsScene& scene);

    undicht::vulkan::TransferBuffer& getTransferBuffer();

    bool newFrame(undicht::vulkan::SwapChain& swap_chain);
    void endFrame(undicht::vulkan::LogicalDevice& device, undicht::vulkan::SwapChain& swap_chain);
    void recreateSwapChain(undicht::vulkan::SwapChain& swap_chain, undicht::vma::VulkanMemoryAllocator& allocator);

};

#endif // PHYSICS_RENDERER_H