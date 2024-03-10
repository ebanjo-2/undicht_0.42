#include "node_animation.h"
#include "glm/gtx/quaternion.hpp"

namespace undicht {

    namespace graphics {

        void NodeAnimation::setNode(const std::string& node) {

            _node = node;
        }

        const std::string& NodeAnimation::getNode() const {

            return _node;
        }

        void NodeAnimation::addPositionKey(double time, const glm::vec3& position) {

            _position_keys[time] = position;
        }

        void NodeAnimation::addRotationKey(double time, const glm::quat& rotation) {

            _rotation_keys[time] = rotation;
        }

        void NodeAnimation::addScaleKey(double time, const glm::vec3& scale) {

            _scale_keys[time] = scale;
        }

        glm::vec3 NodeAnimation::getPosition(double time) const {
            /// @brief  interpolates between the keyframes 
            /// to get the position / rotation / scale at a time between the keyframes

            // get the two keyframes with influence at the current time
            std::pair<double, glm::vec3> pos0 = {0.0, glm::vec3(1.0f)};
            std::pair<double, glm::vec3> pos1 = {0.0, glm::vec3(1.0f)};

            for(const std::pair<double, glm::vec3>& pos : _position_keys) {
                // since the map is a sorted container
                // we are going from the earliest keyframe to the last

                if(pos.first < time)
                    pos0 = pos;

                if(pos.first > time) {
                    pos1 = pos;
                    break;
                }

            }

            // in case the requested time is after the last keyframe
            if(pos1.first < pos0.first) pos1 = pos0;

            // interpolate
            float inter = (time - pos0.first) / (pos1.first - pos0.first);
            return (1.0f - inter) * pos0.second + inter * pos1.second;
        }

        glm::quat NodeAnimation::getRotation(double time) const {
            /// @brief  interpolates between the keyframes 
            /// to get the position / rotation / scale at a time between the keyframes

            // get the two keyframes with influence at the current time
            std::pair<double, glm::quat> rot0 = {0.0, glm::quat()};
            std::pair<double, glm::quat> rot1 = {0.0, glm::quat()};

            for(const std::pair<double, glm::quat>& rot : _rotation_keys) {
                // since the map is a sorted container
                // we are going from the earliest keyframe to the last

                if(rot.first < time)
                    rot0 = rot;

                if(rot.first > time) {
                    rot1 = rot;
                    break;
                }

            }

            // in case the requested time is after the last keyframe
            if(rot1.first < rot0.first) rot1 = rot0;

            // interpolate (spherical linear interpolation "slerp")
            float inter = (time - rot0.first) / (rot1.first - rot0.first);
            return glm::slerp(rot0.second, rot1.second, inter);
        }

        glm::vec3 NodeAnimation::getScale(double time) const {
            /// @brief  interpolates between the keyframes 
            /// to get the position / rotation / scale at a time between the keyframes
            
            // get the two keyframes with influence at the current time
            std::pair<double, glm::vec3> scl0 = {0.0, glm::vec3(1.0f)};
            std::pair<double, glm::vec3> scl1 = {0.0, glm::vec3(1.0f)};

            for(const std::pair<double, glm::vec3>& scl : _scale_keys) {
                // since the map is a sorted container
                // we are going from the earliest keyframe to the last

                if(scl.first < time)
                    scl0 = scl;

                if(scl.first > time) {
                    scl1 = scl;
                    break;
                }

            }

            // in case the requested time is after the last keyframe
            if(scl1.first < scl0.first) scl1 = scl0;

            scl0.second = glm::vec3(1.0f);
            scl1.second = glm::vec3(1.0f);

            // interpolate
            float inter = 1.0f;//(time - scl0.first) / (scl1.first - scl0.first);
            return (1.0f-inter) * scl0.second + inter * scl1.second;
        }

        glm::mat4 NodeAnimation::getTransfMat(double time) const {
            /// @return the interpolated transformation matrix for the node at the requested time

            glm::vec3 pos = getPosition(time);
            glm::quat rot = getRotation(time);
            glm::vec3 scl = getScale(time);

            glm::mat4 transl_mat = glm::translate(glm::mat4(1.0f), pos);
            glm::mat4 rotation_mat = glm::toMat4(rot);
            glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scl);

            return transl_mat * rotation_mat;// * glm::mat4(1.0f);
            //return scale_mat * rotation_mat * transl_mat;
            //return transl_mat * rotation_mat;
            //return rotation_mat * transl_mat;
        }

    } // graphics

} // undicht