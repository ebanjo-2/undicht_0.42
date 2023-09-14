#include "material.h"


namespace undicht {

    namespace graphics {

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

        void Material::genMipMaps(vulkan::CommandBuffer& cmd) {
            // records the commands to generate the mip maps

            Texture* diffuse = (Texture*)getTexture(Texture::Type::DIFFUSE);

            if(diffuse && (diffuse->getImage().getImage() != VK_NULL_HANDLE)) {
                diffuse->genMipMaps(cmd);
            }

        }

    } // graphics

} // undicht