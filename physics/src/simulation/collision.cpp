#include "collision.h"

namespace undicht {

    namespace physics {

        class SphereObject;
        template struct Collision<SphereObject, SphereObject>;

    }

} // undicht