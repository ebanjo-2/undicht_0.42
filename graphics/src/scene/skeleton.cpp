#include "skeleton.h"

namespace undicht {

    namespace graphics {

        void Skeleton::setName(const std::string& name) {
            
            _name = name;
        }

        const std::string& Skeleton::getName() const {
            
            return _name;
        }

        Bone& Skeleton::getRootBone() {
            
            return _root_bone;
        }

        Bone* Skeleton::findBone(const std::string& bone_name) {

            if(!_root_bone.getName().compare(bone_name)) 
                return &_root_bone;
            
            return _root_bone.findChildBone(bone_name, true);
        }

        const glm::mat4& Skeleton::getBoneMatrix(const std::string& bone_name) {

            Bone* bone = findBone(bone_name);
            if(!bone) return _root_bone.getGlobalMatrix();

            return bone->getGlobalMatrix();
        }

        void Skeleton::updateBoneMatrices() {

            _root_bone.updateGlobalMatrix(glm::mat4(1.0f), true);
        }

        void Skeleton::storeBindPose() {
            // store the current pose as bind pose, call updateBoneMatrices first!

            _root_bone.storeBindPose(true);
        }

        void Skeleton::restoreBindPose() {

            _root_bone.restoreBindPose(true);
        }


    } // graphics

} // undicht