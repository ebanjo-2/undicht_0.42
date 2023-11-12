#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "vector"
#include "simulation/collision.h"
#include "objects/sphere_object.h"
#include "objects/cuboid_object.h"

namespace undicht {

    namespace physics {

        /// @brief calculate all collisions between the objects that are going to happen in delta_time seconds
        /// the objects are assumed to continue with their current speed for that time
        /// @param object_times start time for the objects (not 0, if they had a collision before within delta_time)
        void calcCollisions(std::vector<Collision<SphereObject, SphereObject>>& collisions, const std::vector<SphereObject>& objects, const std::vector<float>& object_times, float delta_time);

        /// removes collisions that are "unreachable", because the objects involved will collide with other spheres 
        /// before ever reaching the collision point
        template<typename T1, typename T2>
        void removeUnreachableCollisions(std::vector<Collision<T1, T2>>& collisions);

        template<typename T1, typename T2>
        float findFirstCollisionTime(const std::vector<Collision<T1, T2>>& collisions, bool exclude_stuck_collisions = true);
        
        template<typename T1, typename T2>
        bool sharedCollisionObject(const Collision<T1, T2>& c0, const Collision<T1, T2>& c1);

    } // physics

} // undicht

#endif // COLLISION_DETECTION_H