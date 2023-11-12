#ifndef SPHERE_H
#define SPHERE_H

#include "math/physics_types.h"


namespace undicht {

    namespace physics {

        class Sphere {
          
          protected:
            // defining attributes of a sphere

            // using integer types because they have a fixed precision
            // at every position and for every sphere size
            uint32_t _radius;
            float _mass; // assuming a uniform distribution of mass across the entire sphere
            float _bounce_coeff = 0.0f;


          public:
            // functions for using the sphere

            Sphere() = default;
            Sphere(uint32_t radius, float mass, float bounce_coeff);

            void setRadius(uint32_t radius);
            void setMass(float mass);
            void setBounceCoeff(float bounce_coeff);

            uint32_t getRadius() const;
            float getMass() const;
            float getBounceCoeff() const;

        };

    } // physics

} // undicht

#endif // SPHERE_H