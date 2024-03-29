#include "scene_loader.h"
#include "texture_loader.h"

#include "debug.h"
#include "file_tools.h"

#include "cassert"
#include "glm/gtc/type_ptr.hpp"

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

            // reset the names of nodes that are bones
            _bone_names.clear();

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
            
            load_to.updateGlobalTransformations();
            load_to.updateBoneMatrices();
            load_to.updateNodeUBOs(*_transfer_buffer);

        }

        ///////////////////////////////// non public SceneLoader functions /////////////////////////////////

        const aiScene* SceneLoader::importAssimpScene(Assimp::Importer& importer, const std::string& file_name) const {

            const aiScene *scene = importer.ReadFile(file_name, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);	
            
            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {

                UND_ERROR << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
                return nullptr;
            }

            return scene;
        }

        void SceneLoader::processAssimpScene(const aiScene* assimp_scene, SceneGroup& load_to, const std::string& directory) {

            // process all materials
            for(int i = 0; i < assimp_scene->mNumMaterials; i++) {

                const aiMaterial* ai_material = assimp_scene->mMaterials[i];
                std::string mat_name = ai_material->GetName().C_Str(); // has to be unique (referenced by meshes)
                if(!mat_name.length()) mat_name = "Material " + toStr(load_to.getMaterials().size()); 
                processAssimpMaterial(ai_material, load_to.addMaterial(mat_name), directory);
            }

            // process all meshes
            for(int i = 0; i < assimp_scene->mNumMeshes; i++) {

                const aiMesh* ai_mesh = assimp_scene->mMeshes[i];
                processAssimpMesh(ai_mesh, load_to.addMesh(ai_mesh->mName.C_Str()), load_to.getMaterials().end() - assimp_scene->mNumMaterials);
            }

            // process all nodes (recursive)
            processAssimpNode(assimp_scene->mRootNode, load_to.getRootNode(), load_to);

            // process all animations
            for(int i = 0; i < assimp_scene->mNumAnimations; i++) {

                const aiAnimation* ai_animation = assimp_scene->mAnimations[i];
                processAssimpAnimation(assimp_scene->mAnimations[i], load_to.addAnimation(ai_animation->mName.C_Str()));
            }

            // setting the bind pose for all skeletons
            for(Skeleton& s : load_to.getSkeletons()) {
                s.updateBoneMatrices();
                s.storeBindPose();
            }

        }

        //////////////////////////////////// functions to process meshes //////////////////////////////////////

        void SceneLoader::processAssimpMesh(const aiMesh* assimp_mesh, Mesh& load_to, const std::vector<Material>::iterator& materials) {
            
            // init the mesh
            load_to.init(*_device, *_allocator);

            // processing vertices and faces of the mesh
            processAssimpVertices(assimp_mesh, load_to);
            processAssimpFaces(assimp_mesh, load_to);

            // storing mesh attributes
            load_to.setVertexAttributes(assimp_mesh->HasPositions(), assimp_mesh->HasTextureCoords(0), assimp_mesh->HasNormals(), assimp_mesh->HasTangentsAndBitangents(), assimp_mesh->HasBones());
            load_to.setName(assimp_mesh->mName.C_Str());
            // load_to.setMaterialID(assimp_mesh->mMaterialIndex + material_id_offset);
            load_to.setMaterial(materials[assimp_mesh->mMaterialIndex].getName());

            // storing the bone names in the mesh
            processAssimpMeshBones(assimp_mesh, load_to);

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

        void SceneLoader::processAssimpMeshBones(const aiMesh* assimp_mesh, Mesh& load_to) {

            std::vector<std::string> bone_names;

            // converting the assimp array to a std::vector
            for(int i = 0; i < assimp_mesh->mNumBones; i++)
                bone_names.push_back(assimp_mesh->mBones[i]->mName.C_Str());

            // register the bone names
            _bone_names.insert(bone_names.begin(), bone_names.end());

            // store the bone names in the mesh
            load_to.setBones(bone_names);

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

        void SceneLoader::processAssimpMat4(const aiMatrix4x4& assimp_mat, glm::mat4& load_to) {

            // access: glm::mat4[col][row]
            load_to[0][0] = assimp_mat.a1; load_to[1][0] = assimp_mat.a2; load_to[2][0] = assimp_mat.a3; load_to[3][0] = assimp_mat.a4;
            load_to[0][1] = assimp_mat.b1; load_to[1][1] = assimp_mat.b2; load_to[2][1] = assimp_mat.b3; load_to[3][1] = assimp_mat.b4;
            load_to[0][2] = assimp_mat.c1; load_to[1][2] = assimp_mat.c2; load_to[2][2] = assimp_mat.c3; load_to[3][2] = assimp_mat.c4;
            load_to[0][3] = assimp_mat.d1; load_to[1][3] = assimp_mat.d2; load_to[2][3] = assimp_mat.d3; load_to[3][3] = assimp_mat.d4;

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

		void SceneLoader::processAssimpNode(const aiNode* assimp_node, Node& load_to, SceneGroup& scene_group) {
            
            // store the nodes meshes
            std::vector<std::string> meshes;
            for(int i = 0; i < assimp_node->mNumMeshes; i++) {
                meshes.push_back(scene_group.getMeshes()[assimp_node->mMeshes[i]].getName());
            }

            if(meshes.size() == 1) {
                if(!load_to.getHasVulkanObjects()) load_to.init(*_device, *_allocator, *_node_descriptor_cache);
                load_to.setMesh(meshes[0]);
            } else {
                load_to.addMeshes(meshes, *_device, *_allocator, *_node_descriptor_cache);
            }

            // store the model matrix
            glm::mat4 local;
            processAssimpMat4(assimp_node->mTransformation, local);
            load_to.setLocalTransformation(local);

            // recursivly processing the child nodes
            for(unsigned int i = 0; i < assimp_node->mNumChildren; i++) {

                // is it a bone node?
                if(_bone_names.find(assimp_node->mChildren[i]->mName.C_Str()) != _bone_names.end()) {
                    // this node is the root node of a skeleton
                    Skeleton* skeleton = scene_group.getSkeleton(assimp_node->mName.C_Str());
                    if(!skeleton) skeleton = &scene_group.addSkeleton(assimp_node->mName.C_Str());
                    processAssimpBoneNode(assimp_node, skeleton->getRootBone());
                    continue;
                }

                // if a mesh gets stored, the node will be reinitialized with vulkan objects
                // making sure that the node has a unique name
                std::string node_name = assimp_node->mChildren[i]->mName.C_Str();
                if(load_to.getChildNode(node_name)) { node_name = node_name + " " + toStr(i);}

                processAssimpNode(assimp_node->mChildren[i], load_to.addChildNode(node_name), scene_group);

            }

        }

        void SceneLoader::processAssimpBoneNode(const aiNode* assimp_node, graphics::Bone& load_to) {
            
            // store the name
            load_to.setName(assimp_node->mName.C_Str());

            // store the local transform
            glm::mat4 local_transf;
            processAssimpMat4(assimp_node->mTransformation, local_transf);
            load_to.setLocalMatrix(local_transf);

            // recursivly processing the child bones
            for(unsigned int i = 0; i < assimp_node->mNumChildren; i++) {

                std::string bone_name = assimp_node->mChildren[i]->mName.C_Str();
                processAssimpBoneNode(assimp_node->mChildren[i], *load_to.addChildBone(bone_name));
            }

        }

        ///////////////////////////////////////// functions to process animations /////////////////////////////////////////

        void SceneLoader::processAssimpAnimation(const aiAnimation* assimp_animation, graphics::Animation& load_to) {

            load_to.init();
            load_to.setName(std::string(assimp_animation->mName.C_Str()));
            load_to.setDuration(assimp_animation->mDuration);
            load_to.setTicksPerSecond(assimp_animation->mTicksPerSecond);

            for(int i = 0; i < assimp_animation->mNumChannels; i++) {
                
                aiNodeAnim* assimp_node_anim = assimp_animation->mChannels[i];
                processAssimpNodeAnimation(assimp_node_anim, load_to.addNodeAnimation(assimp_node_anim->mNodeName.C_Str()));
            }

            UND_LOG << "loaded animation " << '"' << load_to.getName() << '"' << "\n";

        }

        void SceneLoader::processAssimpNodeAnimation(const aiNodeAnim* assimp_node_animation, graphics::NodeAnimation& load_to) {

            for(int i = 0; i < assimp_node_animation->mNumPositionKeys; i++) {
                
                aiVectorKey pos_key = assimp_node_animation->mPositionKeys[i];
                load_to.addPositionKey(pos_key.mTime, glm::vec3(pos_key.mValue.x, pos_key.mValue.y, pos_key.mValue.z));
            }

            for(int i = 0; i < assimp_node_animation->mNumRotationKeys; i++) {
                
                aiQuatKey rot_key = assimp_node_animation->mRotationKeys[i];
                load_to.addRotationKey(rot_key.mTime, glm::quat(rot_key.mValue.w, rot_key.mValue.x, rot_key.mValue.y, rot_key.mValue.z));
            }            
            
            for(int i = 0; i < assimp_node_animation->mNumScalingKeys; i++) {
                
                aiVectorKey scl_key = assimp_node_animation->mPositionKeys[i];
                load_to.addScaleKey(scl_key.mTime, glm::vec3(scl_key.mValue.x, scl_key.mValue.y, scl_key.mValue.z));
            }
        }

    } // tools

} // undicht