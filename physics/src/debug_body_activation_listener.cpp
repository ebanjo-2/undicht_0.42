#include "debug_body_activation_listener.h"
#include "debug.h"

using namespace JPH;

namespace undicht {

    namespace physics {

        void DebugBodyActivationListener::OnBodyActivated(const BodyID &inBodyID, uint64 inBodyUserData) {

            UND_LOG << "Body " << inBodyID.GetIndex() << " got activated" << "\n";
        }

        void DebugBodyActivationListener::OnBodyDeactivated(const BodyID &inBodyID, uint64 inBodyUserData) {

            UND_LOG << "Body " << inBodyID.GetIndex() << " went to sleep" << "\n";
        }

    } // physics

} // undicht