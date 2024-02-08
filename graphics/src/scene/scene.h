#ifndef SCENE_H
#define SCENE_H

#include "vulkan_memory_allocator.h"
#include "core/vulkan/logical_device.h"
#include "renderer/vulkan/descriptor_set_cache.h"
#include "renderer/vulkan/transfer_buffer.h"

#include "scene_group.h"

#include "vector"

namespace undicht {

    namespace graphics {

		class Scene {

		  protected:

		  	std::vector<SceneGroup> _groups;
			
		  public:

			void init();
			void cleanUp();

			SceneGroup& addGroup(const std::string& group_name); // calls init() on a newly created group
			Mesh* addMesh(const std::string& group_name, const std::string& mesh_name); // returns nullptr, if the group doesnt exist
			Material* addMaterial(const std::string& group_name, const std::string& mat_name);
			Animation* addAnimation(const std::string& group_name, const std::string& anim_name);

			SceneGroup* getGroup(const std::string& group_name);
			Mesh* getMesh(const std::string& group_name, const std::string& mesh_name);
			Material* getMaterial(const std::string& group_name, const std::string& mat_name);
			Animation* getAnimation(const std::string& group_name, const std::string& anim_name);

			std::vector<SceneGroup>& getGroups();

			// records the commands to generate the mip maps
			// for all textures of the materials
            void genMipMaps(vulkan::CommandBuffer& cmd);
			void updateNodeUBOs(vulkan::TransferBuffer& transfer_buffer);
            void updateBoneMatrices();
            void updateGlobalTransformation();

		};

	} // graphics

} // undicht

#endif // SCENE_H