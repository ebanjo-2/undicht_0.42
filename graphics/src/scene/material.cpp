#include "material.h"


namespace undicht {

    namespace graphics {

        using namespace vulkan;
        
        void Material::init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache) {

            _device_handle = device;
            _allocator_handle = allocator;

            _descriptor_set = descriptor_cache.allocate();

        }

        void Material::cleanUp() {

            for(Texture& t : _textures) t.cleanUp();

        }

        void Material::setName(const std::string& name) {

            _name = name;
        }

        const std::string& Material::getName() const {

            return _name;
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

        void Material::updateDescriptorSet(const vulkan::Sampler& sampler) {
            /// @brief should be called after changes to the resources of the material were made

            Texture* diffuse = (Texture*)getTexture(Texture::Type::DIFFUSE);

            if(diffuse && (diffuse->getImage().getImage() != VK_NULL_HANDLE)) {

                _descriptor_set.bindImage(0, diffuse->getImageView().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, sampler.getSampler());
                _descriptor_set.update();

            }

        }

        const vulkan::DescriptorSet& Material::getDescriptorSet() const {
            /// @return a descriptor set which binds the resources that define the material

            return _descriptor_set;
        }

    } // graphics

} // undicht