#include "scene.h"

namespace undicht {

    namespace graphics {

        using namespace vulkan;
        using namespace vma;

        void Scene::init() {

        }

        void Scene::cleanUp() {
            
            for(SceneGroup& g : _groups) g.cleanUp();
        }

        SceneGroup& Scene::addGroup(const std::string& group_name, const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& node_descriptor_cache) {

            SceneGroup* g = getGroup(group_name);
            if(g) return *g;

            _groups.emplace_back(SceneGroup());
            _groups.back().setName(group_name);
            _groups.back().init(device, allocator, node_descriptor_cache);
            return _groups.back();
        }

		Mesh* Scene::addMesh(const std::string& group_name, const std::string& mesh_name) {
            
            // make sure that the group exists
            SceneGroup* group = getGroup(group_name);
            if(!group) return nullptr;

            return &group->addMesh(mesh_name);
        }

		Material* Scene::addMaterial(const std::string& group_name, const std::string& mat_name) {

            // make sure that the group exists
            SceneGroup* group = getGroup(group_name);
            if(!group) return nullptr;            
            
            return &group->addMaterial(mat_name);
        }

		Animation* Scene::addAnimation(const std::string& group_name, const std::string& anim_name) {

            // make sure that the group exists (and create it if it doesnt)
            SceneGroup* group = getGroup(group_name);
            if(!group) return nullptr;            
            
            return &group->addAnimation(anim_name);
        }

		SceneGroup* Scene::getGroup(const std::string& group_name) {

            for(SceneGroup& g : _groups)
                if(!g.getName().compare(group_name))
                    return &g;

            return nullptr;
        }

		Mesh* Scene::getMesh(const std::string& group_name, const std::string& mesh_name) {

            SceneGroup* group = getGroup(group_name);
            if(!group) return nullptr;

            return group->getMesh(mesh_name);
        }

		Material* Scene::getMaterial(const std::string& group_name, const std::string& mat_name) {

            SceneGroup* group = getGroup(group_name);
            if(!group) return nullptr;

            return group->getMaterial(mat_name);
        }

		Animation* Scene::getAnimation(const std::string& group_name, const std::string& anim_name) {

            SceneGroup* group = getGroup(group_name);
            if(!group) return nullptr;

            return group->getAnimation(anim_name);
        }

		std::vector<SceneGroup>& Scene::getGroups() {
            
            return _groups;
        }

        void Scene::genMipMaps(vulkan::CommandBuffer& cmd) {
			// records the commands to generate the mip maps
			// for all textures of the materials

            for(SceneGroup& g : _groups)
                g.genMipMaps(cmd);

        }

    } // graphics

} // undicht