#include "physics_renderer.h"
#include "glm/gtc/type_ptr.hpp"
#include "debug.h"

using namespace undicht;
using namespace graphics;
using namespace vulkan;

void PhysicsRenderer::init(const LogicalDevice& device, SwapChain& swap_chain, vma::VulkanMemoryAllocator& allocator) {

    _renderer.init(device, swap_chain, allocator);
    _frame_manager.init(device);

    _transfer_buffer.init(allocator, {device.getGraphicsQueueFamily()}, 1000000);

}

void PhysicsRenderer::cleanUp(undicht::vulkan::SwapChain& swap_chain) {
    
    _transfer_buffer.cleanUp();
    _renderer.cleanUp(swap_chain);
    _frame_manager.cleanUp();

}

undicht::vulkan::DescriptorSetCache& PhysicsRenderer::getMaterialDescriptorCache() {

    return _renderer.getMaterialDescriptorCache();
}

undicht::vulkan::DescriptorSetCache& PhysicsRenderer::getNodeDescriptorCache() {

    return _renderer.getNodeDescriptorCache();
}

undicht::vulkan::Sampler& PhysicsRenderer::getMaterialSampler() {

    return _renderer.getMaterialSampler();
}

void PhysicsRenderer::loadCameraMatrices(glm::mat4& view, glm::mat4& proj) {

    _renderer.loadCameraMatrices(glm::value_ptr(view), glm::value_ptr(proj), _transfer_buffer);

}

void PhysicsRenderer::drawScene(PhysicsScene& scene) {

    _renderer.draw(_frame_manager.getDrawCmd(), scene.getScene());
}

undicht::vulkan::TransferBuffer& PhysicsRenderer::getTransferBuffer() {

    return _transfer_buffer;
}

bool PhysicsRenderer::newFrame(undicht::vulkan::SwapChain& swap_chain) {

    _swap_image = _frame_manager.prepareNextFrame(swap_chain);
    _frame_manager.waitForTransferFinished(); // to reuse the transfer buffer

    if(_swap_image == -1) return false;

    // reset & begin the command buffers (yes, every frame)
    _frame_manager.getTransferCmd().resetCommandBuffer();
    _frame_manager.getTransferCmd().beginCommandBuffer(true);
    _frame_manager.getDrawCmd().resetCommandBuffer();
    _frame_manager.getDrawCmd().beginCommandBuffer(true);

    // begin render pass
    _renderer.begin(_frame_manager.getDrawCmd(), _swap_image);

    _transfer_buffer.reset();

    return true;
}

void PhysicsRenderer::endFrame(LogicalDevice& device, SwapChain& swap_chain) {

    // end render pass
    _renderer.end(_frame_manager.getDrawCmd());

    // record transfer commands
    _transfer_buffer.completeTransfers(_frame_manager.getTransferCmd());

    // end command buffers
    _frame_manager.getTransferCmd().endCommandBuffer();
    _frame_manager.getDrawCmd().endCommandBuffer();

    _frame_manager.finishLastFrame(); // wait for previous frame to finish before submitting more work
    _frame_manager.submitFrame(device, swap_chain, true);
}


void PhysicsRenderer::recreateSwapChain(SwapChain& swap_chain, vma::VulkanMemoryAllocator& allocator) {

    _renderer.recreateFramebuffers(allocator, swap_chain);
    _frame_manager.reset();
}