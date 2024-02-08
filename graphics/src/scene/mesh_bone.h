#ifndef MESH_BONE_H
#define MESH_BONE_H

#include <string>
#include "glm/glm.hpp"

namespace undicht {

    namespace graphics {

        class MeshBone {
          
          protected:

            // the name is used to adress the bone in animations
            // it is also shared with the note representing the bone
            // in the armature (skeleton hierarchy)
            std::string _name;
            
            /// explanation taken from assimp
            /// Matrix that transforms from mesh space to bone space in bind pose.
            /// It is sometimes called an inverse-bind matrix,
            /// or inverse bind pose matrix.
            glm::mat4 _offset_matrix;

          public:

            void setName(const std::string& name);
            const std::string& getName() const;

            void setOffsetMatrix(const glm::mat4& offset_matrix);
            const glm::mat4& getOffsetMatrix() const;

        };

    } // graphics

} // undicht

#endif // MESH_BONE_H