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

        }

        /////////////////////////////////// private physics sim functions ///////////////////////////////////

        void PhysicsSimulation::resolveCollisions(float delta_time) {
            /// moves all objects for the specified time while checking for and resolving any collisions
            
            // the time at which each sphere currently sits at
            std::vector<float> sphere_times(_spheres.size(), 0.0f);

            // calculate the collisions that will occur if the spheres move for the specified time
            std::vector<Collision<SphereObject, SphereObject>> collisions = calcCollisions(_spheres, delta_time);
            
            // move every sphere
            for(int i = 0; i < _spheres.size(); i++) {

                if(_sphere_is_removed.at(i)) continue;

                const Collision<SphereObject, SphereObject>* next_collision = getNextCollision(&_spheres.at(i), collisions);

                if(next_collision) {
                    // resolve collision

                    if(next_collision->_object0 == &_spheres.at(i)) {

                        _spheres.at(i).setPosition(next_collision->_obj0_pos);
                        _spheres.at(i).setVelocity(-0.6f * _spheres.at(i).getVelocity());

                    } else {

                        _spheres.at(i).setPosition(next_collision->_obj1_pos);
                        _spheres.at(i).setVelocity(-0.6f * _spheres.at(i).getVelocity());
                    }

                    //_spheres.at(i).update(next_collision->_time);

                } else {
                    // move without collision

                    _spheres.at(i).update(delta_time);
                }
                
            }

        }


        const Collision<SphereObject, SphereObject>* PhysicsSimulation::getNextCollision(SphereObject* sphere, const std::vector<Collision<SphereObject, SphereObject>>& collisions) {
            /// @brief find the next collision involving the sphere
            /// @return nullptr, if none of the collisions involves the sphere

            const Collision<SphereObject, SphereObject>* next_collision = nullptr;

            for(const Collision<SphereObject, SphereObject>& col : collisions) {

                if(!col._will_collide) continue;

                if((col._object0 == sphere) ||  (col._object1 == sphere)) {
                    // found a collision involving the sphere
                    
                    if((!next_collision) || (next_collision->_time > col._time)) {
                        // it is the next collision involving the object

                        next_collision = &col;
                    }

                }

            }

            return next_collision;
        }

    } // physics

} // undicht