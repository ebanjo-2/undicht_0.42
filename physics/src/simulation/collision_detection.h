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

        std::vector<Collision<SphereObject, SphereObject>> calcCollisions(const std::vector<SphereObject>& objects, float delta_time);

    } // physics

} // undicht

#endif // COLLISION_DETECTION_H