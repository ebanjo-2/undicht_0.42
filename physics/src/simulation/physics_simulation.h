#ifndef PHYSICS_SIMULATION_H
#define PHYSICS_SIMULATION_H

#include "objects/sphere_object.h"
#include "vector"

namespace undicht {

    namespace physics {

        class PhysicsSimulation {
          
          protected:

            struct UniversalAcceleration {
                vec3f _a;
            };

            struct ObjectAcceleration {
                vec3f _a;
                Object* _object;
            };

            std::vector<UniversalAcceleration> _universal_accelerations;
            std::vector<ObjectAcceleration> _object_acceleration;

          public:
            // running the simulation

            /** applies the acceleration to all objects of the simulation 
             * (mainly to add gravity)
             * default gravity would be a = 9.81 */
            void addUniversalAcceleration(const vec3f& a);
            void addObjectAcceleration(Object* object, const vec3f& a);

            /** advances the simulation by the specified time
             * @param delta_time unit: seconds 
             * applies & resets all added accelerations */
            void update(float delta_time, std::vector<SphereObject>& spheres);

          protected:
            // private physics sim functions

            /// moves all objects for the specified time while checking for and resolving any collisions
            /// @param sphere_times in order to resolve the collisions, the objects will be moved until they touch the last object they collide with
            /// the time, at which this happens, is stored in sphere_times for each sphere
            void resolveCollisions(float delta_time, std::vector<SphereObject>& spheres, std::vector<float>& sphere_times);



        };

    } // physics

} // undicht

#endif // PHYSICS_SIMULATION_H