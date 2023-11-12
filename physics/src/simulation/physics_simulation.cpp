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
            for(UniversalAcceleration& a : _universal_accelerations) {
                
                vec3f added_vel = a._a * delta_time;
                float added_vel_length = glm::length(added_vel);

                for(SphereObject& s : spheres) {

                    // try to make objects sitting on other objects more stable
                    if(glm::length(s.getVelocity()) < 0.2f * added_vel_length) s.addVelocity(0.1f * added_vel);
                    else s.addVelocity(added_vel);
            
                }

            }

            for(ObjectAcceleration& a : _object_acceleration) {

                vec3f added_vel = a._a * delta_time;
                float added_vel_length = glm::length(added_vel);

                // try to make objects sitting on other objects more stable
                if(glm::length(a._object->getVelocity()) < 0.2f *  added_vel_length) a._object->addVelocity(0.1f * added_vel);
                else a._object->addVelocity(added_vel);

            }

            // resolve collisions
            std::vector<float> sphere_times(spheres.size(), 0.0f);
            resolveCollisions(delta_time, spheres, sphere_times);

            // move every sphere (now knowing they wont collide)
            for(int i = 0; i < spheres.size(); i++) {

                if(delta_time > sphere_times.at(i));
                
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
            std::vector<Collision<SphereObject, SphereObject>> collisions;

            int num_collisions = 0;

            float first_collision_time;
            float prev_first_collision_time = 0.0f;

            do { // resolve all collisions

                collisions.clear();
                calcCollisions(collisions, spheres, sphere_times, delta_time);
                removeUnreachableCollisions(collisions);

                num_collisions += collisions.size();
                first_collision_time = findFirstCollisionTime(collisions);

                // resolve the next collisions
                for(Collision<SphereObject, SphereObject>& c : collisions) {
                    
                    // carry out the collision
                    collide(c);

                    // advance the time for the two objects
                    // slighty cursed way to get the sphere id
                    float& object0_time = sphere_times.at(c._object0 - &spheres.front());
                    float& object1_time = sphere_times.at(c._object1 - &spheres.front());
                    advance(c, delta_time, first_collision_time, prev_first_collision_time, object0_time, object1_time);

                }

                prev_first_collision_time = first_collision_time;

            } while(collisions.size());

            UND_LOG << "resolving " << num_collisions << " collisions \n";

        }

        template<typename T1, typename T2>
        void PhysicsSimulation::collide(const Collision<T1, T2>& c) {
            /// carries out the physics behind the collision
            /// moves the objects to the point where they collided

            // move the objects to the point of collision
            c._object0->setPosition(c._obj0_pos);
            c._object1->setPosition(c._obj1_pos);

            // formula to calculate the new velocities taken from wikipedia https://de.wikipedia.org/wiki/Sto%C3%9F_(Physik)
            float k = glm::min(c._object0->getBounceCoeff(), c._object1->getBounceCoeff());
            float m0 = c._object0->getMass();
            float m1 = c._object1->getMass();
            vec3f v0 = c._object0->getVelocity();
            vec3f v1 = c._object1->getVelocity();
            c._object0->setVelocity((m0 * v0 + m1 * v1 - m1 * (v0 - v1) * k) / (m0 + m1));
            c._object1->setVelocity((m0 * v0 + m1 * v1 - m0 * (v1 - v0) * k) / (m0 + m1));

            // stop really slow moving objects (only after they collided) (optimization)
            if(glm::length(c._object0->getVelocity()) < 0.1f) c._object0->setVelocity(vec3f(0.0f));
            if(glm::length(c._object1->getVelocity()) < 0.1f) c._object1->setVelocity(vec3f(0.0f));

        }

        template<typename T1, typename T2>
        void PhysicsSimulation::advance(const Collision<T1, T2>& c, float delta_time, float first_coll, float prev_first_coll, float& object0_time, float& object1_time) {
            /// advances the object times for the two objects involved in the collision

            // if the two spheres were stuck, they should still move forward in time with the other spheres
            if(c._are_stuck) {
                // move to the first point in time the other objects involved in are
                if(first_coll > prev_first_coll) {

                    object0_time = first_coll;
                    object1_time = first_coll;
                } else {

                    object0_time = c._time + delta_time / 10.0f;
                    object1_time = c._time + delta_time / 10.0f;
                }

            } else {
                // try to avoid rediculous numbers of bounces per object per frame
                object0_time = glm::max(c._time, object0_time + delta_time / 10.0f);
                object1_time = glm::max(c._time, object1_time + delta_time / 10.0f);
            }

        }

        class SphereObject;
        template void PhysicsSimulation::collide<SphereObject, SphereObject>(const Collision<SphereObject, SphereObject>& c);
        template void PhysicsSimulation::advance(const Collision<SphereObject, SphereObject>& c, float delta_time, float first_coll, float prev_first_coll, float& object0_time, float& object1_time);

    } // physics

} // undicht