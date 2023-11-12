#include "sphere.h"

namespace undicht {

    namespace physics {

        Sphere::Sphere( uint32_t radius, float mass, float bounce_coeff) 
        : _radius(radius), _mass(mass), _bounce_coeff(bounce_coeff) {

        }

        void Sphere::setRadius(uint32_t radius) {
            
            _radius = radius;
        }

        void Sphere::setMass(float mass) {
            
            _mass = mass;
        }

        void Sphere::setBounceCoeff(float bounce_coeff) {

            _bounce_coeff = bounce_coeff;
        }

        uint32_t Sphere::getRadius() const {
            
            return _radius;
        }

        float Sphere::getMass() const {

            return _mass;
        }

        float Sphere::getBounceCoeff() const {

            return _bounce_coeff;
        }

    } // physics

} // undicht