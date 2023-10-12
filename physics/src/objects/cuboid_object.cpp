#include "cuboid_object.h"

namespace undicht {

    namespace physics {

        CuboidObject::CuboidObject(const vec3i& position, const vec3i& half_size, float mass) :
        Object::Object(position), Cuboid::Cuboid(half_size, mass) {

        }

    } // physics

} // undicht