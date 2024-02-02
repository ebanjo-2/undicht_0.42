#ifndef SCENE_H
#define SCENE_H

#include "vulkan_memory_allocator.h"
#include "core/vulkan/logical_device.h"
#include "renderer/vulkan/descriptor_set_cache.h"

#include "mesh.h"
#include "texture.h"
#include "material.h"
#include "node.h"
#include "animation.h"

#include "vector"

namespace undicht {

    namespace graphics {

		class Scene {
			/// implementing a scene structure similar to the one described 
			/// by https://learnopengl.com/Model-Loading/Assimp
			/// (assimps scene structure)
		  protected:

			vulkan::LogicalDevice _device_handle;
			vma::VulkanMemoryAllocator _allocator_handle;
			vulkan::DescriptorSetCache _material_descriptor_chache_handle;
			
			std::vector<Mesh> _meshes;
			std::vector<Material> _materials;
			std::vector<Animation> _animations;

			Node _root_node;

		  public:

			void init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& node_descriptor_cache);
			void cleanUp();
			
			Mesh& addMesh();
			Material& addMaterial(vulkan::DescriptorSetCache& descriptor_cache);
			Animation& addAnimation();

			// records the commands to generate the mip maps
			// for all textures of the materials
            void genMipMaps(vulkan::CommandBuffer& cmd);

			Node& getRootNode();
			Mesh& getMesh(uint32_t mesh_id);
			uint32_t getMeshCount() const;
			Material& getMaterial(uint32_t material_id);
			uint32_t getMaterialCount() const;

		};

	} // graphics

} // undicht

#endif // SCENE_H