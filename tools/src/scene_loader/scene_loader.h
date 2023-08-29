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

            void importScene(const std::string& file_name, Scene& load_to);

          protected:
            // non public SceneLoader functions

            const aiScene* importAssimpScene(Assimp::Importer& importer, const std::string& file_name) const;
            void processAssimpScene(const aiScene* assimp_scene, Scene& load_to, const std::string& directory);

			// functions to process meshes
            void processAssimpMesh(const aiMesh* assimp_mesh, Mesh& load_to);
            void processAssimpVertices(const aiMesh* assimp_mesh, Mesh& load_to);        
            void processAssimpFaces(const aiMesh* assimp_mesh, Mesh& load_to);
            void processAssimpVec3(const aiVector3D& assimp_vec, std::vector<ai_real>& load_to);

			// functions to process materials
			void processAssimpMaterial(const aiMaterial* assimp_material, Material& load_to, const std::string& directory);

			// functions to process nodes
			void processAssimpNode(const aiNode* assimp_node, Node& load_to);

        };

	} // tools

} // undicht

#endif // SCENE_LOADER_H