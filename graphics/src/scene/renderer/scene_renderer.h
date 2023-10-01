#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

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
#include "renderer/vulkan/transfer_buffer.h"

#include "scene/renderer/basic/basic_renderer.h"

namespace undicht {

    namespace graphics {

        class SceneRenderer {

          protected:

            vulkan::LogicalDevice _device_handle;

            vulkan::RenderPass _render_pass;
            std::vector<vulkan::Framebuffer> _framebuffers;
            std::vector<vulkan::ImageView> _swap_images;
            std::vector<vulkan::Image> _depth_images;
            std::vector<vulkan::ImageView> _depth_image_views;

            vulkan::DescriptorSetLayout _global_descriptor_layout;
            vulkan::DescriptorSetCache _global_descriptor_cache;
            vulkan::DescriptorSetLayout _material_descriptor_layout;
            vulkan::DescriptorSetCache _material_descriptor_cache;
            vulkan::Sampler _material_sampler;

            vulkan::UniformBuffer _uniform_buffer;
            vulkan::DescriptorSet _global_descriptor_set;

            BasicRenderer _basic_renderer;

          public:

            void init(const vulkan::LogicalDevice& device, vulkan::SwapChain& swap_chain, vma::VulkanMemoryAllocator& allocator);
            void cleanUp(vulkan::SwapChain& swap_chain);

            void recreateFramebuffers(vma::VulkanMemoryAllocator& allocator, vulkan::SwapChain& swap_chain);

            void loadCameraMatrices(float* mat4_view, float* mat4_proj, vulkan::TransferBuffer& transfer_buffer);

            // drawing
            void begin(vulkan::CommandBuffer& cmd, uint32_t swap_image_id);
            void end(vulkan::CommandBuffer& cmd);
            
            /// @return the number of draw calls that were made
            uint32_t draw(vulkan::CommandBuffer& cmd, Scene& scene);
            uint32_t draw(vulkan::CommandBuffer& cmd, Scene& scene, Node& node);

            vulkan::DescriptorSetCache& getMaterialDescriptorCache();
            vulkan::Sampler& getMaterialSampler();

          protected:
            // non public functions 

            void initRenderPass(VkFormat swap_image_format);
            void initDepthImages(vma::VulkanMemoryAllocator& allocator, uint32_t image_count, const VkExtent2D& extent);
            void initFramebuffers(std::vector<vulkan::ImageView> swap_images, const VkExtent2D& extent);
            void initUniformBuffer(vma::VulkanMemoryAllocator& allocator);
            void initDescriptorLayout();
            void initDescriptorCache();
            void initSampler();

            void cleanUpFramebuffers();
            void cleanUpDepthImages();

        };

    } // graphics

} // undicht

#endif // SCENE_RENDERER_H