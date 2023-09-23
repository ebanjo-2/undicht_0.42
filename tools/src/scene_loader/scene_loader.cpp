#include "scene_loader.h"
#include "texture_loader.h"

#include "debug.h"
#include "file_tools.h"

#include "cassert"


namespace undicht {

	namespace tools {

        using namespace graphics;
        using namespace vulkan;

        void SceneLoader::importScene(const std::string& file_name, Scene& load_to, TransferBuffer& transfer_buffer) {
            // following the tutorial: https://learnopengl.com/Model-Loading/Model

            // getting the working directory from the file_name
            std::string directory = getFilePath(file_name);
            
            // creating the assimp importer
            Assimp::Importer importer;

            // reading the file using the assimp library
            const aiScene *assimp_scene = importAssimpScene(importer, file_name);
            if(assimp_scene == nullptr) return;

            // loading the data from the assimp scene into the load_to scene
            processAssimpScene(assimp_scene, load_to, directory, transfer_buffer);

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

        void SceneLoader::processAssimpScene(const aiScene* assimp_scene, Scene& load_to, const std::string& directory, TransferBuffer& transfer_buffer) {

            // process all meshes
            for(int i = 0; i < assimp_scene->mNumMeshes; i++) {

                processAssimpMesh(assimp_scene->mMeshes[i], load_to.addMesh(), transfer_buffer);
            }

            // process all materials
            for(int i = 0; i < assimp_scene->mNumMaterials; i++) {

                processAssimpMaterial(assimp_scene->mMaterials[i], load_to.addMaterial(), directory, transfer_buffer);
            }

            // process all nodes (recursive)
            processAssimpNode(assimp_scene->mRootNode, load_to.getRootNode());

        }

        //////////////////////////////////// functions to process meshes //////////////////////////////////////

        void SceneLoader::processAssimpMesh(const aiMesh* assimp_mesh, Mesh& load_to, TransferBuffer& transfer_buffer) {
            
            // processing vertices and faces of the mesh
            processAssimpVertices(assimp_mesh, load_to, transfer_buffer);
            processAssimpFaces(assimp_mesh, load_to, transfer_buffer);

            // storing mesh attributes
            load_to.setVertexAttributes(assimp_mesh->HasPositions(), assimp_mesh->HasTextureCoords(0), assimp_mesh->HasNormals(), assimp_mesh->HasTangentsAndBitangents());
            load_to.setName(assimp_mesh->mName.C_Str());
            load_to.setMaterialID(assimp_mesh->mMaterialIndex);

        }

        void SceneLoader::processAssimpVertices(const aiMesh* assimp_mesh, Mesh& load_to, TransferBuffer& transfer_buffer) {

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

            load_to.setVertexData((const uint8_t*)vertex_data.data(), vertex_data.size() * sizeof(ai_real), transfer_buffer);
        }

        void SceneLoader::processAssimpFaces(const aiMesh* assimp_mesh, Mesh& load_to, TransferBuffer& transfer_buffer) {

            std::vector<uint32_t> face_ids;

            // going through all faces that make up the mesh
            for(unsigned int i = 0; i < assimp_mesh->mNumFaces; i++) {

                const aiFace& assimp_face = assimp_mesh->mFaces[i];

                // going through all the indices that make up a face
                for(unsigned int j = 0; j < assimp_face.mNumIndices; j++) {

                    face_ids.push_back(assimp_face.mIndices[j]);
                }

            }

            load_to.setIndexData((const uint8_t*)face_ids.data(), face_ids.size() * sizeof(uint32_t), transfer_buffer);
            load_to.setVertexCount(face_ids.size());
        }

        void SceneLoader::processAssimpVec3(const aiVector3D& assimp_vec, std::vector<ai_real>& load_to) {

            load_to.push_back(assimp_vec.x);
            load_to.push_back(assimp_vec.y);
            load_to.push_back(assimp_vec.z);
        }

		/////////////////////////////////////// functions to process Materials ///////////////////////////////////////

        void SceneLoader::processAssimpMaterial(const aiMaterial* assimp_material, Material& load_to, const std::string& directory, TransferBuffer& transfer_buffer) {

            // string to store the textures file name
            aiString file_name;

            // try to load a diffuse texture
            if(assimp_material->GetTextureCount(aiTextureType_DIFFUSE) >= 1) {
                assimp_material->GetTexture(aiTextureType_DIFFUSE, 0, &file_name);
                Texture& diffuse = load_to.addTexture(Texture::Type::DIFFUSE);
                TextureLoader(directory + file_name.C_Str(), diffuse, transfer_buffer);

                UND_LOG << "loaded diffuse texture: " << file_name.C_Str() << "\n";
            }

            // try to load a specular texture
            if(assimp_material->GetTextureCount(aiTextureType_SPECULAR) >= 1) {
                assimp_material->GetTexture(aiTextureType_SPECULAR, 0, &file_name);
                Texture& specular = load_to.addTexture(Texture::Type::SPECULAR);
                TextureLoader(directory + file_name.C_Str(), specular, transfer_buffer);

                UND_LOG << "loaded specular texture: " << file_name.C_Str() << "\n";
            }

        }

        //////////////////////////////////////////// functions to process nodes ////////////////////////////////////////////

		void SceneLoader::processAssimpNode(const aiNode* assimp_node, Node& load_to) {
            
            // store the nodes meshes
            std::vector<uint32_t> meshes;
            meshes.insert(meshes.end(), assimp_node->mMeshes, assimp_node->mMeshes + assimp_node->mNumMeshes);
            load_to.setMeshes(meshes);

            // recursivly processing the child nodes
            for(unsigned int i = 0; i < assimp_node->mNumChildren; i++) {

                processAssimpNode(assimp_node->mChildren[i], load_to.addChildNode());
            }

        }

    } // tools

} // undicht