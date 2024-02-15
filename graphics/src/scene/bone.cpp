#include "bone.h"
#include "algorithm"

namespace undicht {

    namespace graphics {

        bool operator== (const Bone& bone, const std::string& name) {
            // for accessing a bone by its name

            return !bone.getName().compare(name);
        }

        void Bone::setOffsetMatrix(const glm::mat4& offset_matrix) {
            
            _offset_matrix = offset_matrix;
        }

        void Bone::setLocalMatrix(const glm::mat4& local_matrix) {
            
            _local_matrix = local_matrix;
        }

        void Bone::updateGlobalMatrix(const glm::mat4& parent_transf, bool recursive) {
            
            _global_matrix = parent_transf * _local_matrix;
            _bone_matrix = _global_matrix * _offset_matrix;

            // recursivly updating for all children
            if(recursive) {
                for(Bone& child : _child_bones)
                    child.updateGlobalMatrix(_global_matrix);
            }

        }

        void Bone::storeBindPose(bool recursive) {
            // store the current pose as bind pose (recursive for children), call updateBoneMatrix first!

            _offset_matrix = glm::inverse(_global_matrix);
            _local_matrix_bind = _local_matrix;

            // recursivly updating for all children
            if(recursive) {
                for(Bone& child : _child_bones)
                    child.storeBindPose();
            }

        }

        void Bone::restoreBindPose(bool recursive) {
            // return the bone into bind pose (recursive for children) 

            _global_matrix = glm::inverse(_offset_matrix);
            _local_matrix = _local_matrix_bind;

            if(recursive) {
                for(Bone& child : _child_bones)
                    child.restoreBindPose();
            }
            
        }

        const glm::mat4& Bone::getOffsetMatrix() const {
            
            return _offset_matrix;
        }

        const glm::mat4& Bone::getLocalMatrix() const {
            
            return _local_matrix;
        }

        const glm::mat4& Bone::getGlobalMatrix() const {

            return _global_matrix;
        }

        const glm::mat4& Bone::getBoneMatrix() const {

            return _bone_matrix;
        }

        void Bone::setName(const std::string& name) {
            
            _name = name;
        }

        const std::string& Bone::getName() const {
            
            return _name;
        }

        Bone* Bone::addChildBone(const std::string& name) {
            /// Bone names have to be unique, 
            /// so if the bone name was already used
            /// nullptr will be returned
            
            std::vector<Bone>::iterator pos = std::find(_child_bones.begin(), _child_bones.end(), name);

            // bone name is already used
            if(pos != _child_bones.end()) return nullptr;

            _child_bones.emplace_back(Bone());
            _child_bones.back().setName(name);

            return &_child_bones.back();
        }

        Bone* Bone::findChildBone(const std::string& name, bool search_recursive) {

            std::vector<Bone>::iterator pos = std::find(_child_bones.begin(), _child_bones.end(), name);

            if(pos != _child_bones.end()) return &*pos; // as it turns out, &* may do something

            if(search_recursive) {
                for(Bone& child : _child_bones) {
                    Bone* found = child.findChildBone(name);
                    if(found) return found;
                }
            }
        
            return nullptr;
        }

        bool Bone::removeChildBone(const std::string& name) {
            
            std::vector<Bone>::iterator pos = std::find(_child_bones.begin(), _child_bones.end(), name);

            if(pos == _child_bones.end()) return false;

            _child_bones.erase(pos);

            return true;
        }

    } // graphics

} // undicht