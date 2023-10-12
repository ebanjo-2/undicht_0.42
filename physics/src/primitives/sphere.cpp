#include "sphere.h"

namespace undicht {

    namespace physics {

        Sphere::Sphere( uint32_t radius, float mass) 
        : _radius(radius), _mass(mass) {

        }

        void Sphere::setRadius(uint32_t radius) {
            
            _radius = radius;
        }

        void Sphere::setMass(float mass) {
            
            _mass = mass;
        }

        uint32_t Sphere::getRadius() const {
            
            return _radius;
        }

        float Sphere::getMass() const {

            return _mass;
        }

    } // physics

} // undicht