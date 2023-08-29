#include "scene.h"

namespace undicht {

    using namespace vulkan;
    using namespace vma;

    void Scene::init(const LogicalDevice& device, VulkanMemoryAllocator& allocator) {

        _device_handle = device;
        _allocator_handle = allocator;
    }

    void Scene::cleanUp() {

        for(Mesh& m : _meshes) m.cleanUp();
        for(Material& m : _materials) m.cleanUp();

    }

    Mesh& Scene::addMesh() {

        _meshes.emplace_back(Mesh());
        _meshes.back().init(_device_handle, _allocator_handle);

        return _meshes.back();
    }

    Material& Scene::addMaterial() {

        _materials.emplace_back(Material());
        _materials.back().init(_device_handle, _allocator_handle);

        return _materials.back();
    }

    Node& Scene::getRootNode() {

        return _root_node;
    }

} // undicht