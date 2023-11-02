#include "collision_detection.h"
#include "debug.h"

namespace undicht {

    namespace physics {

        /// @brief calculate all collisions between the objects that are going to happen in delta_time seconds
        /// the objects are assumed to continue with their current speed for that time
        /// @param object_times start time for the objects (not 0, if they had a collision before within delta_time)

        std::vector<Collision<SphereObject, SphereObject>> calcCollisions(const std::vector<SphereObject>& objects, const std::vector<float>& object_times, float delta_time) {

            std::vector<Collision<SphereObject, SphereObject>> collisions;

            // checking each object against every other object once
            for(int sphere0 = 0; sphere0 < objects.size(); sphere0++) {

                for(int sphere1 = sphere0 + 1; sphere1 < objects.size(); sphere1++) {
                    
                    const SphereObject& s0 = objects.at(sphere0);
                    const SphereObject& s1 = objects.at(sphere1);

                    // checking if there may be a chance
                    if(!s0.couldCollide(s1, delta_time, object_times.at(sphere0), object_times.at(sphere1), 1))
                        continue;

                    // calculating at what time they collide (if they collide)
                    Collision<SphereObject, SphereObject> col = s0.willCollide(s1, delta_time, object_times.at(sphere0), object_times.at(sphere1));
                    if(col._will_collide)
                        collisions.push_back(col);

                }

            }

            return collisions;
        }

        template<typename T1, typename T2>
        void removeUnreachableCollisions(std::vector<Collision<T1, T2>>& collisions) {
            /// removes collisions that are "unreachable", because the objects involved will collide with other spheres 
            /// before ever reaching the collision point

            for(int i = 0; i < collisions.size(); i++) {
                
                // the next potentionally reachable collision
                Collision<T1, T2> reachable = collisions.at(i);

                // checking if the collision is reachable
                for(int j = i + 1; j < collisions.size(); j++) {
                    
                    // do the collisions have a object in common
                    if((collisions.at(j)._object0 == reachable._object0) || (collisions.at(j)._object0 == reachable._object1) || (collisions.at(j)._object1 == reachable._object0) || (collisions.at(j)._object1 == reachable._object1)) {
                        
                        if(collisions.at(j)._time < reachable._time) {
                            // the new collision happens before the reachable one, making it the next reachable collision
                            reachable = collisions.at(j);
                        }

                        // remove duplicate / unreachable collision
                        collisions.erase(collisions.begin() + j);
                        j--;
                    }
                
                }

                collisions.at(i) = reachable;

            }

        }

        template void removeUnreachableCollisions(std::vector<Collision<SphereObject, SphereObject>>& collisions);

    } // physics

} // undicht