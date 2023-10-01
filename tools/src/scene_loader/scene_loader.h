#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "scene/scene.h"
#include "scene/mesh.h"
#include "scene/texture.h"
#include "scene/material.h"
#include "scene/node.h"

#include "string"
#include "vector"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace undicht {

	namespace tools {

        class SceneLoader {

          public:

            void importScene(const std::string& file_name, graphics::Scene& load_to, vulkan::TransferBuffer& transfer_buffer, vulkan::DescriptorSetCache& material_descriptor_cache, vulkan::Sampler& sampler);

          protected:
            // non public SceneLoader functions

            const aiScene* importAssimpScene(Assimp::Importer& importer, const std::string& file_name) const;
            void processAssimpScene(const aiScene* assimp_scene, graphics::Scene& load_to, const std::string& directory, vulkan::TransferBuffer& transfer_buffer, vulkan::DescriptorSetCache& material_descriptor_cache, vulkan::Sampler& sampler);

			// functions to process meshes
            void processAssimpMesh(const aiMesh* assimp_mesh, graphics::Mesh& load_to, vulkan::TransferBuffer& transfer_buffer);
            void processAssimpVertices(const aiMesh* assimp_mesh, graphics::Mesh& load_to, vulkan::TransferBuffer& transfer_buffer);        
            void processAssimpFaces(const aiMesh* assimp_mesh, graphics::Mesh& load_to, vulkan::TransferBuffer& transfer_buffer);
            void processAssimpVec3(const aiVector3D& assimp_vec, std::vector<ai_real>& load_to);

            // functions to process materials
            void processAssimpMaterial(const aiMaterial* assimp_material, graphics::Material& load_to, const std::string& directory, vulkan::TransferBuffer& transfer_buffer, vulkan::Sampler& sampler);

            // functions to process nodes
            void processAssimpNode(const aiNode* assimp_node, graphics::Node& load_to);

        };

	} // tools

} // undicht

#endif // SCENE_LOADER_H