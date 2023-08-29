#include "material.h"


namespace undicht {

    using namespace vulkan;
    
    void Material::init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator) {

        _device_handle = device;
        _allocator_handle = allocator;

    }

    void Material::cleanUp() {

        for(Texture& t : _textures) t.cleanUp();

    }

    Texture& Material::addTexture(Texture::Type type) {

        _textures.emplace_back(Texture());
        _textures.back().init(_device_handle, _allocator_handle, type);

        return _textures.back();
    }

    const Texture* Material::getTexture(Texture::Type type) const {
        /// @return will return nullptr, if the material doesnt have a texture of the requested type

        for(const Texture& t : _textures) {
            
            if(t.getType() == type) {
                return &t;
            }
        }

        return nullptr;
    }

} // undicht