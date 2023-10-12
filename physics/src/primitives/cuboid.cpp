#include "cuboid.h"

namespace undicht {

    namespace physics {

        Cuboid::Cuboid(const vec3i& half_size, float mass) 
        : _half_size(half_size), _mass(mass) {

        }

        void Cuboid::setHalfSize(const vec3i& half_size) {

            _half_size = half_size;
        }

        void Cuboid::setMass(float mass) {

            _mass = mass;
        }

        const vec3i& Cuboid::getHalfSize() const {

            return _half_size;
        }

        float Cuboid::getMass() const {

            return _mass;
        }

    } // physics

} // undicht