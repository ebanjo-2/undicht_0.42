#include "physics_simulation.h"
#include "collision_detection.h"
#include "debug.h"

namespace undicht {

    namespace physics {

        //////////////////////////////////// managing physics objects ////////////////////////////////////

        uint32_t PhysicsSimulation::addSphereObject(const SphereObject& object) {

            _spheres.push_back(object);
            _sphere_is_removed.push_back(false);
            
            return _spheres.size() - 1;
        }

        SphereObject& PhysicsSimulation::getSphereObject(uint32_t id) {

            return _spheres.at(id);
        }

        void PhysicsSimulation::removeSphereObject(uint32_t id) {

            _sphere_is_removed.at(id) = true;
        }

        const std::vector<SphereObject>& PhysicsSimulation::getSphereObjects() const {

            return _spheres;
        }

        void PhysicsSimulation::clearAllObjects() {
            
            _spheres.clear();
            _sphere_is_removed.clear();

        }

        //////////////////////////////////// running the simulation ////////////////////////////////////

        void PhysicsSimulation::applyUniversalAcceleration(const vec3f& a, float delta_time) {
            /** applies the acceleration to all objects of the simulation 
             * (mainly to add gravity)
             * default gravity would be a = 9.81 */

            for(int i = 0; i < _spheres.size(); i++) {

                if(_sphere_is_removed.at(i)) continue;

                _spheres.at(i).addVelocity(a * delta_time);

            }

        }

        void PhysicsSimulation::update(float delta_time) {
            /** advances the simulation by the specified time
             * @param delta_time unit: seconds */

            resolveCollisions(delta_time);

            for(int i = 0; i < _spheres.size(); i++) {

                if(_sphere_is_removed.at(i)) continue;

                _spheres.at(i).update(delta_time);
                
            }

        }

        /////////////////////////////////// private physics sim functions ///////////////////////////////////

        void PhysicsSimulation::resolveCollisions(float delta_time) {

            // get all collisions
            std::vector<Collision<SphereObject, SphereObject>> collisions = calcCollisions(_spheres, delta_time);
            
            // resolve collisions
            for(const Collision<SphereObject, SphereObject>& collision : collisions) {

                if(collision._will_collide) {

                    SphereObject* s0 = collision._object0;
                    SphereObject* s1 = collision._object1;

                    //s0->setPosition(collision.getPosition());
                    s0->setPosition(collision._obj0_pos);
                    s0->setVelocity(-0.7f * s0->getVelocity());
                    //s0->setVelocity(vec3f(0.0f));

                    s1->setPosition(collision._obj1_pos);
                    s1->setVelocity(-0.7f * s1->getVelocity());

                }

            }

        }

    } // physics

} // undicht