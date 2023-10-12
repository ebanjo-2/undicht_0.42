#ifndef CUBOID_OBJECT_H
#define CUBOID_OBJECT_H

#include "primitives/object.h"
#include "primitives/cuboid.h"

namespace undicht {

    namespace physics {

        class CuboidObject : public Object, public Cuboid {

          public:

            CuboidObject() = default;
            CuboidObject(const vec3i& position, const vec3i& half_size, float mass);

            // collision detection

        };

    } // physics

} // undicht

#endif // CUBOID_OBJECT_H