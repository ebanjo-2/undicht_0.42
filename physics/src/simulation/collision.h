#ifndef COLLISION_H
#define COLLISION_H

#include "math/physics_types.h"

namespace undicht {

    namespace physics {
		
		template<typename T0, typename T1>
        struct Collision {
            /** holds information about the collision of two objects */

		  	T0* _object0 = nullptr;
			T1* _object1 = nullptr;

			bool _will_collide;
			double _time;  // time in seconds till impact

            vec3i _position; // a point at which the two objects collide (may not be the only one)

            vec3i _obj0_pos; // position of the object 0 at the time of collision
            vec3i _obj1_pos; // position of the object 1 at the time of collision
            vec3f _normal0; // surface normal of the first object at the collision point
            vec3f _normal1; // surface normal of the second object at the collision point

        };

    } // physics

} // undich

#endif // COLLISION_H