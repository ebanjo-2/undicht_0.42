#ifndef OBJECT_H
#define OBJECT_H

#include "math/physics_types.h"

namespace undicht {

    namespace physics {

        class Object {
            /// represents the state of a 3D object
            /// its orientation: position and rotation
            /// its energy: mass, velocity and angular momentum

          protected:
            // defining properties of a 3D object

            vec3i _position = vec3i(0);
            quatf _rotation = quatf(vec3f(0.0f));

            vec3f _velocity = vec3f(0.0f);
            quatf _angular_velocity = quatf(vec3f(0.0f));

          public:
            // functions to use an object

            Object() = default;
            Object(const vec3i& position, const quatf& rotation = quatf(vec3f(0,0,0)));

            /** base unit for positions is one tenth of a millimeter (e-4 meters) */
            void setPosition(const vec3i& position);
            void setRotation(const quatf& rotation);
            /** base unit for velocity is meters / second */
            void setVelocity(const vec3f& velocity);
            /** this rotation gets added to the rotation of the object every second */
            void setAngularVelocity(const quatf& angular_velocity);

            void addPosition(const vec3i& position);
            void addRotation(const quatf& rotation);
            void addVelocity(const vec3f& velocity);
            void addAngularVelocity(const quatf& angular_velocity);

            const vec3i& getPosition() const;
            const quatf& getRotation() const;
            const vec3f& getVelocity() const;
            const quatf& getAngularVelocity() const;

            /** moves and rotates the object based on its current velocity and angular velocity 
             * @param delta_time : in seconds */
            void update(float delta_time);

            /// @return a matrix representing the current rotation of the object
            mat4f getRotationMatrix() const;

        };

    } // physics

} // undicht

#endif // OBJECT_H