#include "collision_detection.h"
#include "debug.h"

namespace undicht {

    namespace physics {

        /// @brief calculate all collisions between the objects that are going to happen in delta_time seconds
        /// the objects are assumed to continue with their current speed for that time

        std::vector<Collision<SphereObject, SphereObject>> calcCollisions(const std::vector<SphereObject>& objects, float delta_time) {

            std::vector<Collision<SphereObject, SphereObject>> collisions;

            // checking each object against every other object once
            for(int sphere0 = 0; sphere0 < objects.size(); sphere0++) {

                for(int sphere1 = sphere0 + 1; sphere1 < objects.size(); sphere1++) {
                    
                    const SphereObject& s0 = objects.at(sphere0);
                    const SphereObject& s1 = objects.at(sphere1);

                    // checking if there may be a chance
                    if(!s0.couldCollide(s1, delta_time, 1))
                        continue;

                    // calculating at what time they collide (if they collide)
                    collisions.push_back(s0.willCollide(s1, delta_time));

                }

            }

            return collisions;
        }

    } // physics

} // undicht