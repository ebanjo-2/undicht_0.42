#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include "core/vulkan/descriptor_set.h"
#include "renderer/vulkan/descriptor_set_cache.h"
#include "core/vulkan/sampler.h"

namespace undicht {

    namespace graphics {

        class Material {

          protected:

            vulkan::LogicalDevice _device_handle;
            vma::VulkanMemoryAllocator _allocator_handle;

            // attributes of the Material
            std::vector<Texture> _textures;

            vulkan::DescriptorSet _descriptor_set;

          public:

            void init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache);
            void cleanUp();

            Texture& addTexture(Texture::Type type);

            /// @return will return nullptr, if the material doesnt have a texture of the requested type
            const Texture* getTexture(Texture::Type type) const;

            // records the commands to generate the mip maps
            void genMipMaps(vulkan::CommandBuffer& cmd);

            /// @brief should be called after changes to the resources of the material were made
            void updateDescriptorSet(const vulkan::Sampler& sampler);

            /// @return a descriptor set which binds the resources that define the material
            const vulkan::DescriptorSet& getDescriptorSet() const;

        };

    } // graphics

} // undicht

#endif // MATERIAL_H