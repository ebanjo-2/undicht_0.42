#ifndef RENDERER_H
#define RENDERER_H

#include "scene/scene.h"

#include "vulkan_memory_allocator.h"

#include "core/vulkan/swap_chain.h"
#include "core/vulkan/logical_device.h"
#include "core/vulkan/shader_module.h"
#include "core/vulkan/render_pass.h"
#include "core/vulkan/pipeline.h"
#include "core/vulkan/frame_buffer.h"
#include "core/vulkan/fence.h"
#include "core/vulkan/semaphore.h"
#include "core/vulkan/command_buffer.h"
#include "core/vulkan/sampler.h"
#include "core/vulkan/descriptor_set.h"
#include "core/vulkan/descriptor_set_layout.h"
#include "core/vulkan/descriptor_pool.h"

#include "renderer/vulkan/uniform_buffer.h"
#include "renderer/vulkan/descriptor_set_cache.h"

namespace undicht {

    class Renderer {

      protected:

        vulkan::LogicalDevice _device_handle;

        vulkan::RenderPass _render_pass;
        vulkan::Pipeline _pipeline;
        vulkan::ShaderModule _vertex_shader; // shader modules (might add more later)
        vulkan::ShaderModule _fragment_shader;
        std::vector<vulkan::Framebuffer> _framebuffers;
        std::vector<vulkan::ImageView> _swap_images;
        std::vector<vulkan::Image> _depth_images;
        std::vector<vulkan::ImageView> _depth_image_views;

        vulkan::DescriptorSetLayout _global_descriptor_layout;
        vulkan::DescriptorSetCache _global_descriptor_cache;
        vulkan::DescriptorSetLayout _per_node_descriptor_layout;
        vulkan::DescriptorSetCache _per_node_descriptor_cache;

        // uniform buffers, samplers and descriptors
        vulkan::UniformBuffer _uniform_buffer;
        vulkan::Sampler _sampler;

      public:

        void init(const vulkan::LogicalDevice& device, vulkan::SwapChain& swap_chain, vma::VulkanMemoryAllocator& allocator);
        void cleanUp(vulkan::SwapChain& swap_chain);

        void recreateFramebuffers(vma::VulkanMemoryAllocator& allocator, vulkan::SwapChain& swap_chain);

        void loadCameraMatrices(float* mat4_view, float* mat4_proj);

        // drawing
        void begin(vulkan::CommandBuffer& cmd, vulkan::Semaphore& swap_image_ready, uint32_t swap_image_id);
        void end(vulkan::CommandBuffer& cmd);

        void draw(vulkan::CommandBuffer& cmd, Scene& scene);
        void draw(vulkan::CommandBuffer& cmd, Scene& scene, Node& node);

      protected:
        // non public functions 

        void initShaderModules();
        void initRenderPass(VkFormat swap_image_format);
        void initDepthImages(vma::VulkanMemoryAllocator& allocator, uint32_t image_count, const VkExtent2D& extent);
        void initFramebuffers(std::vector<vulkan::ImageView> swap_images, const VkExtent2D& extent);
        void initPipeline(const VkExtent2D& view_port);
        void initDescriptorLayouts();
        void initDescriptorCaches();

        void initUniformBuffer(vma::VulkanMemoryAllocator& allocator);
        void initSampler();

        void cleanUpFramebuffers();
        void cleanUpDepthImages();

    };

} // undicht

#endif // RENDERER_H