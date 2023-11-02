#include "physics_simulation.h"
#include "collision_detection.h"
#include "debug.h"

namespace undicht {

    namespace physics {

        //////////////////////////////////// running the simulation ////////////////////////////////////

        void PhysicsSimulation::addUniversalAcceleration(const vec3f& a) {
            /** applies the acceleration to all objects of the simulation 
             * (mainly to add gravity)
             * default gravity would be a = 9.81 */

            UniversalAcceleration acc{};
            acc._a = a;
            _universal_accelerations.push_back(acc);

        }

        void PhysicsSimulation::addObjectAcceleration(Object* object, const vec3f& a) {

            ObjectAcceleration acc{};
            acc._object = object;
            acc._a = a;
            _object_acceleration.push_back(acc);

        }

        void PhysicsSimulation::update(float delta_time, std::vector<SphereObject>& spheres) {
            /** advances the simulation by the specified time
             * @param delta_time unit: seconds */

            // apply accelerations (is a little inaccurate if the object collides within delta_time, but should be alright)
            for(UniversalAcceleration& a : _universal_accelerations)
                for(SphereObject& s : spheres)
                    s.addVelocity(a._a * delta_time);

            for(ObjectAcceleration& a : _object_acceleration)
                a._object->addVelocity(a._a * delta_time);

            // resolve collisions
            std::vector<float> sphere_times(spheres.size(), 0.0f);
            resolveCollisions(delta_time, spheres, sphere_times);

            // move every sphere
            for(int i = 0; i < spheres.size(); i++) {
                
                spheres.at(i).update(delta_time - sphere_times.at(i));
            }
            
            _universal_accelerations.clear();
            _object_acceleration.clear();

        }

        /////////////////////////////////// private physics sim functions ///////////////////////////////////

        void PhysicsSimulation::resolveCollisions(float delta_time, std::vector<SphereObject>& spheres, std::vector<float>& sphere_times) {
            /// moves all objects for the specified time while checking for and resolving any collisions
            /// @param sphere_times in order to resolve the collisions, the objects will be moved until they touch the last object they collide with
            /// the time, at which this happens, is stored in sphere_times for each sphere


            // the collisions that need to be resolved
            std::vector<Collision<SphereObject, SphereObject>> collisions = calcCollisions(spheres, sphere_times, delta_time);
            removeUnreachableCollisions(collisions);

            int num_collisions = 0;

            // resolve all collisions
            while(collisions.size()) {

                num_collisions += collisions.size();

                // resolve the current collisions
                for(Collision<SphereObject, SphereObject>& c : collisions) {

                    c._object0->setPosition(c._obj0_pos);
                    c._object1->setPosition(c._obj1_pos);

                    if(glm::length(c._object0->getVelocity()) > 0.01f)
                        c._object0->setVelocity(-0.02f * c._object0->getVelocity());
                    else
                        c._object0->setVelocity(vec3f(0.0f));

                    if(glm::length(c._object1->getVelocity()) > 0.01f)
                        c._object1->setVelocity(-0.02f * c._object1->getVelocity());
                    else
                        c._object1->setVelocity(vec3f(0.0f));

                    // slighty cursed way to get the sphere id from the pointers stored in the collision
                    sphere_times.at((c._object0 - &spheres.front()) / sizeof(SphereObject*)) = c._time;
                    sphere_times.at((c._object1 - &spheres.front()) / sizeof(SphereObject*)) = c._time;

                }

                // find collisions that may occur within delta_time after the previous collisions
                collisions = calcCollisions(spheres, sphere_times, delta_time);
                removeUnreachableCollisions(collisions);
            }

            // UND_LOG << "resolving " << num_collisions << " collisions \n";

        }

    } // physics

} // undicht