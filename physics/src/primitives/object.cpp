#include "object.h"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace undicht {

    namespace physics {


        Object::Object(const vec3i& position, const quatf& rotation) 
        : _position(position), _rotation(rotation) {

        }

        void Object::setPosition(const vec3i& position) {
            /** base unit for positions is one tenth of a millimeter (e-4 meters) */

            _position = position;
        }

        void Object::setRotation(const quatf& rotation) {

            _rotation = rotation;
        }

        void Object::setVelocity(const vec3f& velocity) {
            /** base unit for velocity is meters / second */

            _velocity = velocity;
        }

        void Object::setAngularVelocity(const quatf& angular_velocity) {
            /** this rotation gets added to the rotation of the object every second */

            _angular_velocity = angular_velocity;
        }

        void Object::addPosition(const vec3i& position) {
            
            _position += position;
        }

        void Object::addRotation(const quatf& rotation) {
            
            _rotation = glm::normalize(_rotation * rotation);
        }

        void Object::addVelocity(const vec3f& velocity) {

            _velocity += velocity;
        }

        void Object::addAngularVelocity(const quatf& angular_velocity) {

            // dont normalize here, since there is a difference between rotating 40° per second and 400 degrees per second
            // if this was just the rotation (without speed), there wouldnt be a difference
            _angular_velocity = _angular_velocity * angular_velocity;
        }

        const vec3i& Object::getPosition() const {

            return _position;
        }

        const quatf& Object::getRotation() const {

            return _rotation;
        }

        const vec3f& Object::getVelocity() const {

            return _velocity;
        }

        const quatf& Object::getAngularVelocity() const {

            return _angular_velocity;
        }

        void Object::update(float delta_time) {
            /** moves and rotates the object based on its current velocity and angular velocity 
             * @param delta_time : in seconds */

            // velocity in m/s
            _position += _velocity * 10000.0f * delta_time;

            // angular velocity in radians/s
            _rotation = glm::normalize(_rotation * (_angular_velocity * delta_time));

        }

        mat4f Object::getRotationMatrix() const {
            /// @return a matrix representing the current rotation of the object

            return glm::toMat4(_rotation);
        }

    } // physics

} // undicht