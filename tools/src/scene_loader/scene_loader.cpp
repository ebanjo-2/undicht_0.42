#include "scene_loader.h"
#include "texture_loader.h"

#include "debug.h"
#include "file_tools.h"

#include "cassert"

namespace undicht {

	namespace tools {

        using namespace graphics;
        using namespace vulkan;

        void SceneLoader::setInitObjects(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::TransferBuffer& transfer_buffer, vulkan::DescriptorSetCache& material_descriptor_cache, vulkan::DescriptorSetCache& node_descriptor_cache, const vulkan::Sampler& sampler) {
            
            _device = &device;
            _transfer_buffer = &transfer_buffer;
            _material_descriptor_cache = &material_descriptor_cache;
            _node_descriptor_cache = &node_descriptor_cache;
            _sampler = &sampler;
            _allocator = &allocator;
        }

        void SceneLoader::importScene(const std::string& file_name, SceneGroup& load_to) {
            // following the tutorial: https://learnopengl.com/Model-Loading/Model

            if(!_device) {
                UND_ERROR << "SceneLoader is not initialized (call setInitObjects())\n";
                return;
            }

            // getting the working directory from the file_name
            std::string directory = getFilePath(file_name);
            
            // creating the assimp importer
            Assimp::Importer importer;

            // reading the file using the assimp library
            const aiScene *assimp_scene = importAssimpScene(importer, file_name);
            if(assimp_scene == nullptr) {
                UND_ERROR << "failed to load assimp scene: " << file_name << "\n";
                return;
            }

            // loading the data from the assimp scene into the load_to scene
            processAssimpScene(assimp_scene, load_to, directory);

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

        void SceneLoader::processAssimpScene(const aiScene* assimp_scene, SceneGroup& load_to, const std::string& directory) {
            

            // process all meshes
            for(int i = 0; i < assimp_scene->mNumMeshes; i++) {

                const aiMesh* ai_mesh = assimp_scene->mMeshes[i];
                processAssimpMesh(ai_mesh, load_to.addMesh(ai_mesh->mName.C_Str()), (const aiMaterial**)assimp_scene->mMaterials);
            }

            // process all materials
            for(int i = 0; i < assimp_scene->mNumMaterials; i++) {

                const aiMaterial* ai_material = assimp_scene->mMaterials[i];
                processAssimpMaterial(ai_material, load_to.addMaterial(ai_material->GetName().C_Str()), directory);
            }

            // process all nodes (recursive)
            processAssimpNode(assimp_scene->mRootNode, load_to.getRootNode(), (const aiMesh**)assimp_scene->mMeshes);

            // process all animations
            for(int i = 0; i < assimp_scene->mNumAnimations; i++) {

                const aiAnimation* ai_animation = assimp_scene->mAnimations[i];
                processAssimpAnimation(assimp_scene->mAnimations[i], load_to.addAnimation(ai_animation->mName.C_Str()));
            }

        }

        //////////////////////////////////// functions to process meshes //////////////////////////////////////

        void SceneLoader::processAssimpMesh(const aiMesh* assimp_mesh, Mesh& load_to, const aiMaterial** materials) {
            
            // init the mesh
            load_to.init(*_device, *_allocator);

            // processing vertices and faces of the mesh
            processAssimpVertices(assimp_mesh, load_to);
            processAssimpFaces(assimp_mesh, load_to);

            // storing mesh attributes
            load_to.setVertexAttributes(assimp_mesh->HasPositions(), assimp_mesh->HasTextureCoords(0), assimp_mesh->HasNormals(), assimp_mesh->HasTangentsAndBitangents(), assimp_mesh->HasBones());
            load_to.setName(assimp_mesh->mName.C_Str());
            // load_to.setMaterialID(assimp_mesh->mMaterialIndex + material_id_offset);
            load_to.setMaterial(materials[assimp_mesh->mMaterialIndex]->GetName().C_Str());

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
                if(assimp_mesh->HasBones()) processAssimpVertexBones(assimp_mesh, i, vertex_data);

            }

            load_to.setVertexData((const uint8_t*)vertex_data.data(), vertex_data.size() * sizeof(ai_real), *_transfer_buffer);
        }

        void SceneLoader::processAssimpFaces(const aiMesh* assimp_mesh, Mesh& load_to) {

            std::vector<uint32_t> face_ids;

            // going through all faces that make up the mesh
            for(unsigned int i = 0; i < assimp_mesh->mNumFaces; i++) {

                const aiFace& assimp_face = assimp_mesh->mFaces[i];

                // going through all the indices that make up a face
                for(unsigned int j = 0; j < assimp_face.mNumIndices; j++) {

                    face_ids.push_back(assimp_face.mIndices[j]);
                }

            }

            load_to.setIndexData((const uint8_t*)face_ids.data(), face_ids.size() * sizeof(uint32_t), *_transfer_buffer);
            load_to.setVertexCount(face_ids.size());
        }

        void SceneLoader::processAssimpVertexBones(const aiMesh* assimp_mesh, int vertex_id, std::vector<ai_real>& load_to) {
            
            const int MAX_BONES_PER_VERTEX = 4;
            std::vector<int> bone_ids;
            std::vector<aiVertexWeight*> weights; // weights of the bones that effect the vertex

            // find all (or MAX_BONES_PER_VERTEX, whichever number is greater) bones that effect the vertex
            for(int bone_id = 0; bone_id < assimp_mesh->mNumBones; bone_id++) {

                aiBone* bone = assimp_mesh->mBones[bone_id];

                // does the bone effect the vertex ?
                for(int bone_vertex = 0; bone_vertex < bone->mNumWeights; bone_vertex++) {
                    if(bone->mWeights[bone_vertex].mVertexId == vertex_id) {
                        bone_ids.push_back(bone_id);
                        weights.push_back(bone->mWeights + bone_vertex); // yes, the bone effects the vertex
                        break;
                    }
                }

            }

            // store the bone indices
            const int no_index = 0; // fallback, for when not enough bones effect the vertex
            for(int i = 0; i < MAX_BONES_PER_VERTEX; i++) {
                if(i < weights.size()) load_to.push_back(*(float*)&bone_ids[i]); // storing the int disguised as a float
                else load_to.push_back(*(float*)&no_index);
            }

            // store the bone weights
            const float no_weight = 0; // fallback, for when not enough bones effect the vertex
            for(int i = 0; i < MAX_BONES_PER_VERTEX; i++) {
                if(i < weights.size()) load_to.push_back(weights[i]->mWeight);
                else load_to.push_back(no_weight);
            }

        }

        void SceneLoader::processAssimpVec3(const aiVector3D& assimp_vec, std::vector<ai_real>& load_to) {

            load_to.push_back(assimp_vec.x);
            load_to.push_back(assimp_vec.y);
            load_to.push_back(assimp_vec.z);
        }

		/////////////////////////////////////// functions to process Materials ///////////////////////////////////////

        void SceneLoader::processAssimpMaterial(const aiMaterial* assimp_material, Material& load_to, const std::string& directory) {
            
            // init the material
            load_to.init(*_device, *_allocator, *_material_descriptor_cache);

            // string to store the textures file name
            aiString file_name;

            // try to load a diffuse texture
            if(assimp_material->GetTextureCount(aiTextureType_DIFFUSE) >= 1) {
                assimp_material->GetTexture(aiTextureType_DIFFUSE, 0, &file_name);
                Texture& diffuse = load_to.addTexture(Texture::Type::DIFFUSE);
                TextureLoader(directory + file_name.C_Str(), diffuse, *_transfer_buffer);

                UND_LOG << "loaded diffuse texture: " << file_name.C_Str() << "\n";
            }

            // try to load a specular texture
            if(assimp_material->GetTextureCount(aiTextureType_SPECULAR) >= 1) {
                assimp_material->GetTexture(aiTextureType_SPECULAR, 0, &file_name);
                Texture& specular = load_to.addTexture(Texture::Type::SPECULAR);
                TextureLoader(directory + file_name.C_Str(), specular, *_transfer_buffer);

                UND_LOG << "loaded specular texture: " << file_name.C_Str() << "\n";
            }

            load_to.updateDescriptorSet(*_sampler);

        }

        //////////////////////////////////////////// functions to process nodes ////////////////////////////////////////////

		void SceneLoader::processAssimpNode(const aiNode* assimp_node, Node& load_to, const aiMesh** scene_meshes) {
            
            // store the nodes meshes
            std::vector<std::string> meshes;
            for(int i = 0; i < assimp_node->mNumMeshes; i++) {
                meshes.push_back(scene_meshes[assimp_node->mMeshes[i]]->mName.C_Str());
            }
                
            load_to.setMeshes(meshes);

            // store the model matrix
            load_to.setModelMatrix((uint8_t*)&assimp_node->mTransformation, *_transfer_buffer);

            // recursivly processing the child nodes
            for(unsigned int i = 0; i < assimp_node->mNumChildren; i++) {

                processAssimpNode(assimp_node->mChildren[i], load_to.addChildNode(assimp_node->mName.C_Str(), *_device, *_allocator, *_node_descriptor_cache), scene_meshes);
            }

        }

        ///////////////////////////////////////// functions to process animations /////////////////////////////////////////

        void SceneLoader::processAssimpAnimation(const aiAnimation* assimp_animation, graphics::Animation& load_to) {

            load_to.init();
            load_to.setName(std::string(assimp_animation->mName.C_Str()));

            UND_LOG << "loaded animation " << '"' << load_to.getName() << '"' << "\n";

        }

    } // tools

} // undicht