#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"

namespace undicht {

    class Material {

      protected:

        vulkan::LogicalDevice _device_handle;
        vma::VulkanMemoryAllocator _allocator_handle;

        // attributes of the Material
        std::vector<Texture> _textures;

      public:

        void init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator);
        void cleanUp();

        Texture& addTexture(Texture::Type type);

        /// @return will return nullptr, if the material doesnt have a texture of the requested type
        const Texture* getTexture(Texture::Type type) const;

    };

} // undicht

#endif // MATERIAL_H