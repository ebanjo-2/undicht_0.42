#include "mesh_bone.h"

namespace undicht {

    namespace graphics {

        void MeshBone::setName(const std::string& name) {

            _name = name;
        }

        const std::string& MeshBone::getName() const {

            return _name;
        }

        void MeshBone::setOffsetMatrix(const glm::mat4& offset_matrix) {

            _offset_matrix = offset_matrix;
        }

        const glm::mat4& MeshBone::getOffsetMatrix() const {

            return _offset_matrix;
        }

    } // graphics

} // undicht