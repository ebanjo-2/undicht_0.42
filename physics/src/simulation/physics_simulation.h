#ifndef PHYSICS_SIMULATION_H
#define PHYSICS_SIMULATION_H

#include "objects/sphere_object.h"
#include "vector"

namespace undicht {

    namespace physics {

        class PhysicsSimulation {
          
          protected:

            std::vector<SphereObject> _spheres;
            std::vector<bool> _sphere_is_removed;

          public:
            // managing physics objects

            uint32_t addSphereObject(const SphereObject& object);
            SphereObject& getSphereObject(uint32_t id);
            void removeSphereObject(uint32_t id);
            const std::vector<SphereObject>& getSphereObjects() const;

            void clearAllObjects();

            // running the simulation

            /** applies the acceleration to all objects of the simulation 
             * (mainly to add gravity)
             * default gravity would be a = 9.81 */
            void applyUniversalAcceleration(const vec3f& a, float delta_time);

            /** advances the simulation by the specified time
             * @param delta_time unit: seconds */
            void update(float delta_time);

          protected:
            // private physics sim functions

            void resolveCollisions(float delta_time);

        };

    } // physics

} // undicht

#endif // PHYSICS_SIMULATION_H