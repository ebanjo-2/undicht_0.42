#include "scene.h"

namespace undicht {

    namespace graphics {

        using namespace vulkan;
        using namespace vma;

        void Scene::init(const LogicalDevice& device, VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& node_descriptor_cache) {

            _device_handle = device;
            _allocator_handle = allocator;

            _root_node.init(device, allocator, node_descriptor_cache);

        }

        void Scene::cleanUp() {

            for(Mesh& m : _meshes) m.cleanUp();
            for(Material& m : _materials) m.cleanUp();

            _root_node.cleanUp();

            _meshes.clear();
            _materials.clear();

        }

        Mesh& Scene::addMesh() {

            _meshes.emplace_back(Mesh());
            _meshes.back().init(_device_handle, _allocator_handle);

            return _meshes.back();
        }

        Material& Scene::addMaterial(vulkan::DescriptorSetCache& material_descriptor_cache) {

            _materials.emplace_back(Material());
            _materials.back().init(_device_handle, _allocator_handle, material_descriptor_cache);

            return _materials.back();
        }

        void Scene::genMipMaps(vulkan::CommandBuffer& cmd) {
            // records the commands to generate the mip maps

            for(Material& m : _materials)
                m.genMipMaps(cmd);

        }

        Node& Scene::getRootNode() {

            return _root_node;
        }

        Mesh& Scene::getMesh(uint32_t mesh_id) {

            return _meshes.at(mesh_id);
        }

        Material& Scene::getMaterial(uint32_t material_id) {

            return _materials.at(material_id);
        }

		uint32_t Scene::getMaterialCount() const {
            
            return _materials.size();
        }

    } // graphics

} // undicht