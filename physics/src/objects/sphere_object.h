#ifndef SPHERE_OBJECT_H
#define SPHERE_OBJECT_H

#include "primitives/object.h"
#include "primitives/sphere.h"
#include "math/physics_types.h"
#include "simulation/collision.h"

namespace undicht {

    namespace physics {

        class SphereObject : public Object, public Sphere {

          public:

            SphereObject() = default;
            SphereObject(const vec3i& position, uint32_t radius, float mass, float bounce_coeff);

			// collision detection
            
            /// @brief detect if two spheres are occupying the same space (which shouldnt happen)
            bool isColliding(const SphereObject& other, Collision<SphereObject, SphereObject>& info, float sphere0_time, float sphere1_time) const;

            /// @brief constructs spheres around the paths that the two spheres are going to take
            /// @param resolution determines the amount of spheres that are going to be used
            bool couldCollide(const SphereObject& other, float delta_time, float sphere0_time, float sphere1_time, int32_t resolution = 1) const;

			/// @brief calculates collision info for this object and the other one
			/// *this object is object 0
			bool willCollide(const SphereObject& other, Collision<SphereObject, SphereObject>& info, float delta_time, float sphere0_time, float sphere1_time) const;

        };

        // general sphere math

        bool overlap(const vec3i& pos0, uint32_t r0, const vec3i& pos1, uint32_t r1);
        bool overlap(const vec3f& pos0, float r0, const vec3f& pos1, float r1);

    } // physics

} // undicht

#endif // SPHERE_OBJECT_H