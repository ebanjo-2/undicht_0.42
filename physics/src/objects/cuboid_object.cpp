#include "cuboid_object.h"

namespace undicht {

    namespace physics {

        CuboidObject::CuboidObject(const vec3i& position, const vec3i& half_size, float mass, float bounce_coeff) :
        Object::Object(position), Cuboid::Cuboid(half_size, mass, bounce_coeff) {

        }

        bool CuboidObject::isColliding(const SphereObject& sphere, Collision<SphereObject, SphereObject>& info) const {

            

        }

    } // physics

} // undicht