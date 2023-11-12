#include "debug_contact_listener.h"
#include "debug.h"

using namespace JPH;

namespace undicht {

    namespace physics {

        ValidateResult DebugContactListener::OnContactValidate(const  Body &inBody1, const Body &inBody2, RVec3Arg inBaseOffset, const CollideShapeResult &inCollisionResult) {
		    
            UND_LOG << "Contact validate callback" << "\n";

            // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
            return ValidateResult::AcceptAllContactsForThisBodyPair;
        }

        void DebugContactListener::OnContactAdded(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold,  ContactSettings &ioSettings) {
		    
            UND_LOG << "A contact was added" << "\n";
        }

        void DebugContactListener::OnContactPersisted(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold,  ContactSettings &ioSettings) {
		    
            UND_LOG << "A contact was persisted" << "\n";
        }

        void DebugContactListener::OnContactRemoved(const SubShapeIDPair &inSubShapePair) {
            
            UND_LOG << "A contact was removed" << "\n";
        }

    } // physics

} // undicht