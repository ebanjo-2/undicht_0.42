#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "scene.h"
#include "mesh.h"

#include "string"
#include "vector"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace undicht {

    class SceneLoader {

      protected:

      public:

        void importScene(const std::string& file_name, Scene& load_to);
      

      protected:
        // non public SceneLoader functions

        const aiScene* importAssimpScene(Assimp::Importer& importer, const std::string& file_name) const;

        // functions to store data from assimp objects into undicht objects
        void processAssimpScene(const aiScene* assimp_scene, Scene& load_to);
        void processAssimpMesh(const aiMesh* assimp_mesh, Mesh& load_to);
        void processAssimpVertices(const aiMesh* assimp_mesh, Mesh& load_to);        
        void processAssimpFaces(const aiMesh* assimp_mesh, Mesh& load_to);

        void processAssimpVec3(const aiVector3D& assimp_vec, std::vector<ai_real>& load_to);
    };

} // undicht

#endif // SCENE_LOADER_H