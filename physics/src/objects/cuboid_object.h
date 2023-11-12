#ifndef CUBOID_OBJECT_H
#define CUBOID_OBJECT_H

#include "primitives/object.h"
#include "primitives/cuboid.h"
#include "objects/sphere_object.h"
#include "simulation/collision.h"

namespace undicht {

    namespace physics {

        class CuboidObject : public Object, public Cuboid {

          public:

            CuboidObject() = default;
            CuboidObject(const vec3i& position, const vec3i& half_size, float mass, float bounce_coeff);

            // collision detection (only with spheres, cuboid to cuboid collisions are to complicated)
            bool isColliding(const SphereObject& sphere, Collision<SphereObject, SphereObject>& info) const;

        };

    } // physics

} // undicht

#endif // CUBOID_OBJECT_H