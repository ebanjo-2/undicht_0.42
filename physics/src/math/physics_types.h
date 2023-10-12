#ifndef PHYSICS_TYPES_H
#define PHYSICS_TYPES_H

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "cstdint"

namespace undicht {

    namespace physics {

        // type of vector used in the undicht physics library
        typedef glm::detail::tvec3<int32_t> vec3i;
        typedef glm::detail::tvec3<int64_t> vec3l;
        typedef glm::detail::tvec3<_Float32> vec3f;
        typedef glm::detail::tvec3<_Float64> vec3d;

        typedef glm::detail::tquat<_Float32> quatf;

        typedef glm::detail::tmat4x4<float> mat4f;
        typedef glm::detail::tmat3x3<float> mat3f;

    } // physics

} // undicht

#endif // PHYSICS_TYPES_H