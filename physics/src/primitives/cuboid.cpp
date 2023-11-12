#include "cuboid.h"

namespace undicht {

    namespace physics {

        Cuboid::Cuboid(const vec3i& half_size, float mass, float bounce_coeff) 
        : _half_size(half_size), _mass(mass), _bounce_coeff(bounce_coeff) {

        }

        void Cuboid::setHalfSize(const vec3i& half_size) {

            _half_size = half_size;
        }

        void Cuboid::setMass(float mass) {

            _mass = mass;
        }

        void Cuboid::setBounceCoeff(float bounce_coeff) {

            _bounce_coeff = bounce_coeff;
        }

        const vec3i& Cuboid::getHalfSize() const {

            return _half_size;
        }

        float Cuboid::getMass() const {

            return _mass;
        }

        float Cuboid::getBounceCoeff() const {

            return _bounce_coeff;
        }

    } // physics

} // undicht