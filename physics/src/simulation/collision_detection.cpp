#include "collision_detection.h"
#include "debug.h"

namespace undicht {

    namespace physics {

        /// @brief calculate all collisions between the objects that are going to happen in delta_time seconds
        /// the objects are assumed to continue with their current speed for that time
        /// @param object_times start time for the objects (not 0, if they had a collision before within delta_time)

        void calcCollisions(std::vector<Collision<SphereObject, SphereObject>>& collisions, const std::vector<SphereObject>& objects, const std::vector<float>& object_times, float delta_time) {

            Collision<SphereObject, SphereObject> col;

            // checking each object against every other object once
            for(int sphere0 = 0; sphere0 < objects.size(); sphere0++) {

                for(int sphere1 = sphere0 + 1; sphere1 < objects.size(); sphere1++) {
                    
                    const SphereObject& s0 = objects.at(sphere0);
                    const SphereObject& s1 = objects.at(sphere1);

                    // only check for collisions within delta_time
                    if((object_times.at(sphere0) >= delta_time) && (object_times.at(sphere1) >= delta_time))
                        continue;

                    // checking if there may be a chance
                    if(!s0.couldCollide(s1, delta_time, object_times.at(sphere0), object_times.at(sphere1), 1))
                        continue;

                    // calculating at what time they collide (if they collide)
                    if(s0.willCollide(s1, col, delta_time, object_times.at(sphere0), object_times.at(sphere1))) {
                        collisions.push_back(col);
                        continue;
                    }

                    // check for stuck spheres
                    if(s0.isColliding(s1, col, object_times.at(sphere0), object_times.at(sphere1))) {
                        collisions.push_back(col);
                        continue;
                    }

                }

            }

        }

        template<typename T1, typename T2>
        void removeUnreachableCollisions(std::vector<Collision<T1, T2>>& collisions) {
            /// removes collisions that are "unreachable", because the objects involved will collide with other spheres 
            /// before ever reaching the collision point

            for(int i = 0; i < collisions.size(); i++) {

                for(int j = i + 1; j < collisions.size(); j++) {

                    if(sharedCollisionObject(collisions[i], collisions[j])) {

                        if(collisions[i]._time < collisions[j]._time) {
                            // remove collision j
                            collisions.erase(collisions.begin() + j);
                            j--;
                        } else {
                            // remove collision i
                            collisions.erase(collisions.begin() + i);
                            i--;
                            continue;
                        }

                    }

                }

            }

        }
        
        template<typename T1, typename T2>
        float findFirstCollisionTime(const std::vector<Collision<T1, T2>>& collisions, bool exclude_stuck_collisions) {
            
            if(!collisions.size()) return 0.0f;

            float first_collision_time = 0.0f;

            for(const Collision<T1, T2>& c : collisions) {
                
                if(c._are_stuck && exclude_stuck_collisions) continue;

                if((first_collision_time == 0.0f) || (c._time < first_collision_time))
                    first_collision_time = c._time;
            }

            return first_collision_time;
        }

        template<typename T1, typename T2>
        bool sharedCollisionObject(const Collision<T1, T2>& c0, const Collision<T1, T2>& c1) {

            if(c0._object0 == c1._object0)
                return true;

            if(c0._object0 == c1._object1)
                return true;            
            
            if(c0._object1 == c1._object0)
                return true;

            if(c0._object1 == c1._object1)
                return true;

            return false;
        }

        template void removeUnreachableCollisions(std::vector<Collision<SphereObject, SphereObject>>& collisions);
        template float findFirstCollisionTime(const std::vector<Collision<SphereObject, SphereObject>>& collisions, bool exclude_stuck_collisions);
        template bool sharedCollisionObject(const Collision<SphereObject, SphereObject>& c0, const Collision<SphereObject, SphereObject>& c1);

    } // physics

} // undicht