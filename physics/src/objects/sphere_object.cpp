#include "sphere_object.h"
#include "cmath"

namespace undicht {

    namespace physics {

        SphereObject::SphereObject(const vec3i& position, uint32_t radius, float mass) 
        : Object::Object(position), Sphere::Sphere(radius, mass) {

        }

		/////////////////////////////////////// collision detection ///////////////////////////////////////

        bool SphereObject::couldCollide(const SphereObject& other, float delta_time, int32_t resolution) const {
            /// @brief constructs spheres around the paths that the two spheres are going to take
            /// @param resolution determines the amount of spheres that are going to be used

            vec3f pos0 = (vec3f(getPosition()) / 10000.0f);
            vec3f pos1 = (vec3f(other.getPosition()) / 10000.0f);
            float rad0 = (getRadius() / 10000.0f) + glm::length(getVelocity()) * delta_time / 2.0f / resolution;
            float rad1 = (other.getRadius() / 10000.0f) + glm::length(other.getVelocity()) * delta_time / 2.0f / resolution;
            vec3f inc0 = getVelocity() * delta_time / float(resolution + 1);
            vec3f inc1 = other.getVelocity() * delta_time / float(resolution + 1);

            for(int i = 0; i < resolution; i++) {

                if(overlap(pos0 + inc0 * float(i + 1), rad0, pos1 + inc1 * float(i + 1), rad1))
                    return true;

            }

            return false;
        }

		Collision<SphereObject, SphereObject> SphereObject::willCollide(const SphereObject& other, float delta_time) const {
			/// @brief calculates collision info for this object and the other one
			/// *this object is object 0

            Collision<SphereObject, SphereObject> collision_info;
            collision_info._will_collide = false;

            // i did some math, this is the result (so no guaranties)
            // (calculating at which time the two paths get close enough for the two spheres to touch)
            // assuming that both spheres are close enough for 32 bit floats to be big enough for this (check with could collide)
            float delta_x = (getPosition().x - other.getPosition().x) / 10000.0f;
            float delta_y = (getPosition().y - other.getPosition().y) / 10000.0f;
            float delta_z = (getPosition().z - other.getPosition().z) / 10000.0f;
            float delta_v_x = (getVelocity().x - other.getVelocity().x); // * 10000.0
            float delta_v_y = (getVelocity().y - other.getVelocity().y); // * 10000.0
            float delta_v_z = (getVelocity().z - other.getVelocity().z); // * 10000.0
            float min_dist = (getRadius() + other.getRadius()) / 10000.0f;
            float delta_x_squared = delta_x * delta_x;
            float delta_y_squared = delta_y * delta_y;
            float delta_z_squared = delta_z * delta_z;
            float delta_v_x_squared = delta_v_x * delta_v_x;
            float delta_v_y_squared = delta_v_y * delta_v_y;
            float delta_v_z_squared = delta_v_z * delta_v_z;
            float min_dist_squared = min_dist * min_dist;

            float vel_len_sqared = delta_v_x_squared + delta_v_y_squared + delta_v_z_squared;
            if(vel_len_sqared < 0.0000001) return collision_info; // dont divide by zero

            // p-q formula
            float p = (delta_x * delta_v_x + delta_y * delta_v_y + delta_z * delta_v_z) / vel_len_sqared; // half of real p
            float q = (delta_x_squared + delta_y_squared + delta_z_squared - min_dist_squared) / vel_len_sqared;

            float tmp0 = p * p - q; // (p / 2.0) * (p / 2.0) - q

            if(tmp0 <= 0.0) return collision_info; // no real solution (literally)
                
            // these two times should describe the interval in which the two spheres overlap
            float tmp1 = glm::sqrt(tmp0);
            float time0 = - p - tmp1;
            float time1 = - p + tmp1;

            float time = glm::max(time0, 0.0f);

            // the two objects wont reach each other in the specified time
            if((time1 < 0.0f) || (time > delta_time)) return collision_info;

            // calculating normal and position of the collision
            vec3i sphere0_pos = getPosition() + vec3i(getVelocity() * 10000.0f * time); // positions of the spheres at the time of collision
            vec3i sphere1_pos = other.getPosition() + vec3i(other.getVelocity() * 10000.0f * time);
            vec3i position = sphere1_pos + vec3i(vec3f(sphere0_pos - sphere1_pos) * float(other.getRadius()) / float(getRadius() + other.getRadius()));
            vec3f normal = glm::normalize(vec3f(sphere1_pos - sphere0_pos));

            // filling out the collision info
            collision_info._will_collide = true;
            collision_info._object0 = (SphereObject*)this;
            collision_info._object1 = (SphereObject*)&other;
            collision_info._time = time;
            collision_info._position = position;
            collision_info._obj0_pos = sphere0_pos;
            collision_info._obj1_pos = sphere1_pos;
            collision_info._normal0 = normal;
            collision_info._normal1 = -1.0f * normal;

            return collision_info;
        }

        /////////////////////////////////////// general sphere math ///////////////////////////////////////
        
        bool overlap(const vec3i& pos0, uint32_t r0, const vec3i& pos1, uint32_t r1) {

            vec3l distance = vec3l(pos0 - pos1);
            uint32_t radius_sum = r0 + r1;

            return ((distance.x * distance.x + distance.y * distance.y + distance.z * distance.z) < (radius_sum * radius_sum));
        }

        bool overlap(const vec3f& pos0, float r0, const vec3f& pos1, float r1) {

            return glm::length(pos0 - pos1) < (r0 + r1);
        }


    } // physics

} // undicht