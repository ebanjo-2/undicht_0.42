#ifndef BONE_H
#define BONE_H

#include "string"
#include "vector"
#include "glm/glm.hpp"

namespace undicht {

    namespace graphics {

        class Bone {
            /// bone (or "joint") used in skeletal animation
            /// the position and orientation of a bone influences certain vertices of a model
            /// the "global" coordinate system refers to the models base coordinate system
          protected:

            // the name is used to adress the bone in animations
            std::string _name;

            // transformation of the global coord. system from the bone system in bind pose 
            // (inverse of the transformation from the global coord. system 
            //  to the bone coord. system in "bind" pose)
            glm::mat4 _offset_matrix = glm::mat4(1.0f);

            // transformation of the local coordinate system from the parent system
            glm::mat4 _local_matrix = glm::mat4(1.0f);

            // storing the local transformation in bind pose
            glm::mat4 _local_matrix_bind = glm::mat4(1.0f);

            // transformation of the local coordinate system from the global system
            glm::mat4 _global_matrix;

            // transformation from bind pose to current bone pose
            // used in the vertex shader
            glm::mat4 _bone_matrix = glm::mat4(1.0f);

            // child bones, which can only move relative to the parent bone
            std::vector<Bone> _child_bones;

          public:

            void setOffsetMatrix(const glm::mat4& offset_matrix);
            void setLocalMatrix(const glm::mat4& local_matrix);
            void updateGlobalMatrix(const glm::mat4& parent_transf, bool recursive = true); // recursivly updating for all children, updates the bone matrix as well
            void storeBindPose(bool recursive = true); // store the current pose as bind pose, call updateBoneMatrix first!
            void restoreBindPose(bool recursive = true); // return the bone into bind pose

            const glm::mat4& getOffsetMatrix() const;
            const glm::mat4& getLocalMatrix() const;
            const glm::mat4& getGlobalMatrix() const;
            const glm::mat4& getBoneMatrix() const;

            void setName(const std::string& name);
            const std::string& getName() const;

            /// Bone names have to be unique, 
            /// so if the bone name was already used
            /// nullptr will be returned
            Bone* addChildBone(const std::string& name);
            Bone* findChildBone(const std::string& name, bool search_recursive = true);
            bool removeChildBone(const std::string& name); // @return false, if the child node wasnt found (not recursive!)

        };

        // for accessing a bone by its name
        bool operator== (const Bone& bone, const std::string& name);

    } // graphics

} // undicht

#endif // BONE_H