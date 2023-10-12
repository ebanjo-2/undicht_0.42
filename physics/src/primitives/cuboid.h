#ifndef CUBOID_H
#define CUBOID_H

#include "cstdint"

#include "math/physics_types.h"

namespace undicht {

    namespace physics {

        class Cuboid {

          protected:

            // half the size of the cuboid
            // (distance from the center to its edges)
            vec3i _half_size;
            float _mass; // assuming uniform mass distribution

          public:

            Cuboid(const vec3i& half_size, float mass);

            void setHalfSize(const vec3i& half_size);
            void setMass(float mass);

            const vec3i& getHalfSize() const;
            float getMass() const;

        };

    } // physics

} // undicht

#endif // CUBOID_H