#ifndef NODE_ANIMATION_H
#define NODE_ANIMATION_H

#include "string"
#include "vector"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "map"

namespace undicht {

    namespace graphics {

        class NodeAnimation {

          protected:

            std::string _node; // name of the bone that is affected

            // the keys are sorted by their timestamp
            // by the map container
            // these keys are absolute position / rotation / scale, not relative to the nodes parent
            std::map<double, glm::vec3> _position_keys;
            std::map<double, glm::quat> _rotation_keys;
            std::map<double, glm::vec3> _scale_keys;

          public:

            void setNode(const std::string& node);
            const std::string& getNode() const;

            void addPositionKey(double time, const glm::vec3& position);
            void addRotationKey(double time, const glm::quat& rotation);
            void addScaleKey(double time, const glm::vec3& scale);

            /// @brief  interpolates between the keyframes 
            /// to get the position / rotation / scale at a time between the keyframes
            glm::vec3 getPosition(double time) const;
            glm::quat getRotation(double time) const;
            glm::vec3 getScale(double time) const;

            /// @return the interpolated transformation matrix for the node at the requested time
            glm::mat4 getTransfMat(double time) const;

        };

    } // graphics

} // undicht

#endif // NODE_ANIMATION_H