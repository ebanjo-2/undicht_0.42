#ifndef SKELETON_H
#define SKELETON_H

#include "bone.h"

#include "string"
#include "glm/glm.hpp"

namespace undicht {

    namespace graphics {

        class Skeleton {
            /// storing a bone hierarchy for skeletal animation
            /// calculates the transformation matrices from global to bone space
            /// which can be used in a vertex shader to move vertices for a pose during an animation

          protected:
            
            std::string _name;
            Bone _root_bone;

          public:

            void setName(const std::string& name);
            const std::string& getName() const;

            Bone& getRootBone();
            Bone* findBone(const std::string& bone_name);
            const glm::mat4& getBoneMatrix(const std::string& bone_name);

            void updateBoneMatrices();
            void storeBindPose(); // store the current pose as bind pose, call updateBoneMatrices first!
            void restoreBindPose();

        };

    } // graphics

} // undicht

#endif // SKELETON_H