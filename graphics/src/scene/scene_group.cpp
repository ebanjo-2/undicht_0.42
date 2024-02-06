#include "scene_group.h"

namespace undicht {

    namespace graphics {

        void SceneGroup::init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& node_descriptor_cache) {

            _root_node.init(device, allocator, node_descriptor_cache);
        }

        void SceneGroup::cleanUp() {
            // will call cleanUp() on all objects belonging to the group

            _root_node.cleanUp();

            for(Mesh& m : _meshes) m.cleanUp();
            for(Material& m : _materials) m.cleanUp();
            for(Animation& a : _animations) a.cleanUp();

            _meshes.clear();
            _materials.clear();
            _animations.clear();

        }

        void SceneGroup::setName(const std::string& name) {

            _name = name;
        }

        const std::string& SceneGroup::getName() const {

            return _name;
        }

        Mesh& SceneGroup::addMesh(const std::string& mesh_name) {
            /** @brief adds a Mesh object to the internal array of meshes
             * will not call init() on the mesh object */

            _meshes.emplace_back(Mesh());
            _meshes.back().setName(mesh_name);

            return _meshes.back();
        }

        Material& SceneGroup::addMaterial(const std::string& mat_name) {

            _materials.emplace_back(Material());
            _materials.back().setName(mat_name);

            return _materials.back();
        }

        Animation& SceneGroup::addAnimation(const std::string& anim_name) {

            _animations.emplace_back(Animation());
            _animations.back().setName(anim_name);

            return _animations.back();
        }
            
        Node& SceneGroup::getRootNode() {

            return _root_node;
        }

        const std::vector<Mesh>& SceneGroup::getMeshes() const {

            return _meshes;
        }

		const std::vector<Material>& SceneGroup::getMaterials() const {

            return _materials;
        }

		const std::vector<Animation>& SceneGroup::getAnimations() const {

            return _animations;
        }

        Mesh* SceneGroup::getMesh(const std::string& mesh_name) {
            /** @brief tries to find a mesh with the given name
            * @return nullptr, if no mesh with a matching mesh_name could be found */

            for(Mesh& m : _meshes) 
                if(!m.getName().compare(mesh_name))
                    return &m;

            return nullptr;
        }

        Material* SceneGroup::getMaterial(const std::string& mat_name) {

            for(Material& m : _materials) 
                if(!m.getName().compare(mat_name))
                    return &m;

            return nullptr;
        }

        Animation* SceneGroup::getAnimation(const std::string& anim_name) {
            
            for(Animation& a : _animations) 
                if(!a.getName().compare(anim_name))
                    return &a;

            return nullptr;
        }

        void SceneGroup::genMipMaps(vulkan::CommandBuffer& cmd) {
            // records the commands to generate the mip maps

            for(Material& m : _materials)
                m.genMipMaps(cmd);

        }

    } // graphics

} // undicht