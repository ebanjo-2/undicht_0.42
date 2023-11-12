#ifndef DEBUG_CONTACT_LISTENER_H
#define DEBUG_CONTACT_LISTENER_H

#include "physics.h"
#include "Jolt/Physics/Collision/ContactListener.h"

namespace undicht {

    namespace physics {

        class DebugContactListener : public JPH::ContactListener {

          public:

            // See: ContactListener
            virtual JPH::ValidateResult OnContactValidate(const  JPH::Body &inBody1, const  JPH::Body &inBody2,  JPH::RVec3Arg inBaseOffset, const  JPH::CollideShapeResult &inCollisionResult) override;

            virtual void OnContactAdded(const JPH::Body &inBody1, const  JPH::Body &inBody2, const  JPH::ContactManifold &inManifold,  JPH::ContactSettings &ioSettings) override;

            virtual void OnContactPersisted(const JPH::Body &inBody1, const  JPH::Body &inBody2, const  JPH::ContactManifold &inManifold,  JPH::ContactSettings &ioSettings) override;

            virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override;

        };

    } // physics

} // undicht

#endif // DEBUG_CONTACT_LISTENER_H