#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "scene/scene.h"
#include "scene/mesh.h"
#include "scene/texture.h"
#include "scene/material.h"
#include "scene/node.h"
#include "scene/animation.h"
#include "scene/skeleton.h"

#include "string"
#include "vector"
#include "set"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace undicht {

	namespace tools {

        class SceneLoader {

          protected:

            // vulkan objects to use when creating scene objects
            const vulkan::LogicalDevice* _device = nullptr;
            vulkan::TransferBuffer* _transfer_buffer = nullptr;
            vulkan::DescriptorSetCache* _material_descriptor_cache = nullptr;
            vulkan::DescriptorSetCache* _node_descriptor_cache = nullptr;
            const vulkan::Sampler* _sampler = nullptr;
            vma::VulkanMemoryAllocator* _allocator = nullptr;

            // storing the names of bones referenced by meshes
            // to identify and seperate normal nodes from bone nodes
            std::set<std::string> _bone_names;

          public:

            // store references to the objects
            // that the loader should use when initializing vulkan objects
            void setInitObjects(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::TransferBuffer& transfer_buffer, vulkan::DescriptorSetCache& material_descriptor_cache, vulkan::DescriptorSetCache& node_descriptor_cache, const vulkan::Sampler& sampler);

            void importScene(const std::string& file_name, graphics::SceneGroup& load_to);

          protected:
            // non public SceneLoader functions

            const aiScene* importAssimpScene(Assimp::Importer& importer, const std::string& file_name) const;
            void processAssimpScene(const aiScene* assimp_scene, graphics::SceneGroup& load_to, const std::string& directory);

		        // functions to process meshes
            void processAssimpMesh(const aiMesh* assimp_mesh, graphics::Mesh& load_to, const std::vector<graphics::Material>& materials);
            void processAssimpVertices(const aiMesh* assimp_mesh, graphics::Mesh& load_to);        
            void processAssimpFaces(const aiMesh* assimp_mesh, graphics::Mesh& load_to);
            void processAssimpMeshBones(const aiMesh* assimp_mesh, graphics::Mesh& load_to);
            void processAssimpVertexBones(const aiMesh* assimp_mesh, int vertex_id, std::vector<ai_real>& load_to);
            void processAssimpVec3(const aiVector3D& assimp_vec, std::vector<ai_real>& load_to);
            void processAssimpMat4(const aiMatrix4x4& assimp_mat, glm::mat4& load_to);

            // functions to process materials
            void processAssimpMaterial(const aiMaterial* assimp_material, graphics::Material& load_to, const std::string& directory);

            // functions to process nodes
            void processAssimpNode(const aiNode* assimp_node, graphics::Node& load_to, graphics::SceneGroup& scene_group);
            void processAssimpBoneNode(const aiNode* assimp_node, graphics::Bone& load_to);

            // functions to process animations
            void processAssimpAnimation(const aiAnimation* assimp_animation, graphics::Animation& load_to);

        };

	} // tools

} // undicht

#endif // SCENE_LOADER_H