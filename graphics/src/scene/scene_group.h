#ifndef SCENE_GROUP_H
#define SCENE_GROUP_H

#include "mesh.h"
#include "texture.h"
#include "material.h"
#include "node.h"
#include "animation.h"
#include "skeleton.h"

#include "vector"
#include "string"

namespace undicht {

    namespace graphics {

        class SceneGroup {
            /** contains a group of meshes, materials, animations, nodes, ... 
             * implementing a scene structure similar to assimps scene structure
            */
          protected:

            std::string _name;

			std::vector<Mesh> _meshes;
			std::vector<Material> _materials;
			std::vector<Animation> _animations;
            std::vector<Skeleton> _skeletons; // spooky & scary

			Node _root_node; // contains child nodes

          public:

            void init(); // initializes the root node
            void cleanUp(); // but will call cleanUp() on all objects belonging to the group

            void setName(const std::string& name);
            const std::string& getName() const;

            /** @brief adds a Mesh object to the internal array of meshes
             * will not call init() on the mesh object */
            Mesh& addMesh(const std::string& mesh_name);
            Material& addMaterial(const std::string& mat_name);
            Animation& addAnimation(const std::string& anim_name);
            Skeleton& addSkeleton(const std::string& skel_name);
            
            Node& getRootNode();
            std::vector<Mesh>& getMeshes();
			std::vector<Material>& getMaterials();
			std::vector<Animation>& getAnimations();
			std::vector<Skeleton>& getSkeletons();

            /** @brief tries to find a mesh with the given name
             * @return nullptr, if no mesh with a matching mesh_name could be found */
            Mesh* getMesh(const std::string& mesh_name);
            Material* getMaterial(const std::string& mat_name);
            Animation* getAnimation(const std::string& anim_name);
            Skeleton* getSkeleton(const std::string& skel_name);
            Bone* getBone(const std::string& bone_name); // bone names should be unique across all skeletons

            // records the commands to generate the mip maps
			// for all textures of the materials
            void genMipMaps(vulkan::CommandBuffer& cmd);
			void updateNodeUBOs(vulkan::TransferBuffer& transfer_buffer);
            void updateBoneMatrices();
            void updateGlobalTransformations();
            void updateAnimations(double time);

        };

    } // graphics

} // undicht

#endif // SCENE_GROUP_H