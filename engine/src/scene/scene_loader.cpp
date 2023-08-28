#include "scene_loader.h"

#include "debug.h"
#include "cassert"


namespace undicht {

    void SceneLoader::importScene(const std::string& file_name, Scene& load_to) {
        // following the tutorial: https://learnopengl.com/Model-Loading/Model
        
        // creating the assimp importer
        Assimp::Importer importer;

        // reading the file using the assimp library
        const aiScene *assimp_scene = importAssimpScene(importer, file_name);
        if(assimp_scene == nullptr) return;

        // loading the data from the assimp scene into the load_to scene
        processAssimpScene(assimp_scene, load_to);

    }

    ///////////////////////////////// non public SceneLoader functions /////////////////////////////////

    const aiScene* SceneLoader::importAssimpScene(Assimp::Importer& importer, const std::string& file_name) const {

        const aiScene *scene = importer.ReadFile(file_name, aiProcess_Triangulate | aiProcess_FlipUVs);	
        
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {

            UND_ERROR << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
            return nullptr;
        }

        return scene;
    }

    ///////////////////// functions to store data from assimp objects into undicht objects //////////////

    void SceneLoader::processAssimpScene(const aiScene* assimp_scene, Scene& load_to) {

        // process all meshes
        for(int i = 0; i < assimp_scene->mNumMeshes; i++) {

            processAssimpMesh(assimp_scene->mMeshes[i], load_to.addMesh());
        }

    }

    void SceneLoader::processAssimpMesh(const aiMesh* assimp_mesh, Mesh& load_to) {
        
        // processing vertices and faces of the mesh
        processAssimpVertices(assimp_mesh, load_to);
        processAssimpFaces(assimp_mesh, load_to);

        // storing mesh attributes
        load_to.setVertexAttributes(assimp_mesh->HasPositions(), assimp_mesh->HasTextureCoords(0), assimp_mesh->HasNormals(), assimp_mesh->HasTangentsAndBitangents());


    }

    void SceneLoader::processAssimpVertices(const aiMesh* assimp_mesh, Mesh& load_to) {

        std::vector<ai_real> vertex_data;

        // processing all vertices of the mesh
        for(int i = 0; i < assimp_mesh->mNumVertices; i++) {

            // processing all possible vertex attributes in the order specified in mesh.h
            if(assimp_mesh->HasPositions()) processAssimpVec3(assimp_mesh->mVertices[i], vertex_data);
            if(assimp_mesh->HasTextureCoords(0)) processAssimpVec3(assimp_mesh->mTextureCoords[0][i], vertex_data); // only one texture coord per vertex for now
            if(assimp_mesh->HasNormals()) processAssimpVec3(assimp_mesh->mNormals[i], vertex_data);
            if(assimp_mesh->HasTangentsAndBitangents()) {
                processAssimpVec3(assimp_mesh->mTangents[i], vertex_data);
                processAssimpVec3(assimp_mesh->mBitangents[i], vertex_data);
            }

        }

        load_to.setVertexData((const char*)vertex_data.data(), vertex_data.size() * sizeof(ai_real));
    }

    void SceneLoader::processAssimpFaces(const aiMesh* assimp_mesh, Mesh& load_to) {

        std::vector<uint32_t> face_ids;

        // going through all faces that make up the mesh
        for(int i = 0; i < assimp_mesh->mNumFaces; i++) {

            const aiFace& assimp_face = assimp_mesh->mFaces[i];

            // going through all the indices that make up a face
            for(int j = 0; j < assimp_face.mNumIndices; j++) {

                face_ids.push_back(assimp_face.mIndices[j]);
            }

        }

        load_to.setIndexData((const char*)face_ids.data(), face_ids.size() * sizeof(uint32_t));
    }

    void SceneLoader::processAssimpVec3(const aiVector3D& assimp_vec, std::vector<ai_real>& load_to) {

        load_to.push_back(assimp_vec.x);
        load_to.push_back(assimp_vec.y);
        load_to.push_back(assimp_vec.z);
    }


} // undicht