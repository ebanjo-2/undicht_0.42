#include "sphere_object.h"
#include "cmath"

namespace undicht {

    namespace physics {

        SphereObject::SphereObject(const vec3i& position, uint32_t radius, float mass, float bounce_coeff) 
        : Object::Object(position), Sphere::Sphere(radius, mass, bounce_coeff) {

        }

		/////////////////////////////////////// collision detection ///////////////////////////////////////

        bool SphereObject::isColliding(const SphereObject& other, Collision<SphereObject, SphereObject>& info, float sphere0_time, float sphere1_time) const {
            /// @brief detect if two spheres are occupying the same space (which shouldnt happen)
            
            vec3l rel_pos = vec3l(other.getPosition()) - vec3l(getPosition());

            // bringing both spheres to the same time
            if(sphere0_time < sphere1_time) rel_pos -= getVelocity() * (sphere1_time - sphere0_time);
            else rel_pos += other.getVelocity() * (sphere0_time - sphere1_time);

            uint32_t dst_len = glm::length(vec3d(rel_pos));
            const uint32_t allowed_overlap = 10;

            if((dst_len + allowed_overlap) < (getRadius() + other.getRadius())) {
                
                // calculate positions so that the two spheres are no longer stuck
                vec3f normal = glm::normalize(vec3f(rel_pos));
                vec3i position = getPosition() + vec3i(normal * float(getRadius() - ((getRadius() + other.getRadius()) - dst_len) / 2) + vec3f(0.5f));
                vec3i sphere0_pos = position - vec3i(normal * float(getRadius()) + vec3f(0.5f));
                vec3i sphere1_pos = position + vec3i(normal * float(other.getRadius()) + vec3f(0.5f));

                // fill out the collision info
                info._will_collide = true;
                info._are_stuck = true;
                info._object0 = (SphereObject*)this;
                info._object1 = (SphereObject*)&other;
                info._time = glm::max(sphere0_time, sphere1_time);
                info._position = position;
                info._obj0_pos = sphere0_pos;
                info._obj1_pos = sphere1_pos;
                info._normal0 = normal;
                info._normal1 = -1.0f * normal;

                return true;
            } else {

                return false;
            }

        }

        bool SphereObject::couldCollide(const SphereObject& other, float delta_time, float sphere0_time, float sphere1_time, int32_t resolution) const {
            /// @brief constructs spheres around the paths that the two spheres are going to take
            /// @param resolution determines the amount of spheres that are going to be used

            vec3f rel_pos = (vec3f(other.getPosition() - getPosition()) / 10000.0f);
            vec3f rel_vel = other.getVelocity() - getVelocity();

            // moving the spheres to the same time
            if(sphere0_time > sphere1_time) {
                rel_pos += other.getVelocity() * (sphere0_time - sphere1_time);
                delta_time -= sphere0_time;
            } else {
                rel_pos -= getVelocity() * (sphere1_time - sphere0_time);
                delta_time -= sphere1_time;
            }

            // the two spheres are moving away from each other -> they cant collide
            if(glm::dot(rel_pos, rel_vel) > 0.0f) return false;

            // check if their two paths could intersect (by contructing spheres around them)
            float rad0 = (getRadius() / 10000.0f) + glm::length(getVelocity()) * delta_time / 2.0f / resolution;
            float rad1 = (other.getRadius() / 10000.0f) + glm::length(other.getVelocity()) * delta_time / 2.0f / resolution;
            vec3f inc0 = getVelocity() * delta_time / float(resolution + 1);
            vec3f inc1 = other.getVelocity() * delta_time / float(resolution + 1);

            for(int i = 0; i < resolution; i++) {

                if(overlap(inc0 * float(i + 1), rad0, rel_pos + inc1 * float(i + 1), rad1))
                    return true;

            }

            return false;
        }

		bool SphereObject::willCollide(const SphereObject& other, Collision<SphereObject, SphereObject>& info, float delta_time, float sphere0_time, float sphere1_time) const {
			/// @brief calculates collision info for this object and the other one
			/// *this object is object 0

            // i did some math, this is the result (so no guaranties)
            // (calculating at which time the two paths get close enough for the two spheres to touch)
            // assuming that both spheres are close enough for 32 bit floats to be big enough for this (check with couldCollide())

            // helper variables
            vec3f delta = vec3f(getPosition() - other.getPosition()) / 10000.0f;
            vec3f delta_squared = delta * delta;
            vec3f delta_v = getVelocity() - other.getVelocity();
            vec3f delta_v_squared = delta_v * delta_v;
            float min_dist = (getRadius() + other.getRadius()) / 10000.0f;
            float min_dist_squared = min_dist * min_dist;

            float vel_len_sqared = delta_v_squared.x + delta_v_squared.y + delta_v_squared.z;
            if(vel_len_sqared < 0.000001f) return false; // avoid dividing by zero

            // moving the spheres to the same time
            if(sphere0_time > sphere1_time) {
                delta -= other.getVelocity() * (sphere0_time - sphere1_time);
                delta_time -= sphere0_time;
            } else {
                delta += getVelocity() * (sphere1_time - sphere0_time);
                delta_time -= sphere1_time;
            }

            if(delta_time <= 0.0f) return false;

            // p-q formula to solve for the two times where the spheres are at a distance equal to the sum of their radii
            float p = (delta.x * delta_v.x + delta.y * delta_v.y + delta.z * delta_v.z) / vel_len_sqared; // half of real p
            float q = (delta_squared.x + delta_squared.y + delta_squared.z - min_dist_squared) / vel_len_sqared;

            float tmp0 = p * p - q; // (p / 2.0) * (p / 2.0) - q

            if(tmp0 <= 0.0f) return false; // no real solution (literally)
                
            // these two times should describe the interval in which the two spheres overlap
            float tmp1 = glm::sqrt(tmp0);
            float time0 = - p - tmp1;
            float time1 = - p + tmp1;

            float time = glm::max(time0, 0.0f);

            // the two objects wont reach each other in the specified time
            if((time1 < 0.0f) || (time > delta_time)) return false;

            // calculating normal and position of the collision
            vec3i sphere0_pos = getPosition() + vec3i(getVelocity() * 10000.0f * time); // positions of the spheres at the time of collision
            vec3i sphere1_pos = other.getPosition() + vec3i(other.getVelocity() * 10000.0f * time);
            vec3i position = sphere1_pos + vec3i(vec3f(sphere0_pos - sphere1_pos) * float(other.getRadius()) / float(getRadius() + other.getRadius()));
            vec3f normal = glm::normalize(vec3f(sphere1_pos - sphere0_pos));

            // filling out the collision info
            info._will_collide = true;
            info._are_stuck = false;
            info._object0 = (SphereObject*)this;
            info._object1 = (SphereObject*)&other;
            info._time = time;
            info._position = position;
            info._obj0_pos = sphere0_pos;
            info._obj1_pos = sphere1_pos;
            info._normal0 = normal;
            info._normal1 = -1.0f * normal;

            return true;
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