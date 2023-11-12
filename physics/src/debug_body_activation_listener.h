#ifndef DEBUG_BODY_ACTIVATION_LISTENER_H
#define DEBUG_BODY_ACTIVATION_LISTENER_H

#include "physics.h"
#include <Jolt/Physics/Body/BodyActivationListener.h>

namespace undicht {

    namespace physics {

        class DebugBodyActivationListener : public JPH::BodyActivationListener {

          public:

            virtual void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override;

            virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override;

        };

    } // physics

} // undicht

#endif // DEBUG_BODY_ACTIVATION_LISTENER_H