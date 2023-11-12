#include "physics.h"
#include "debug.h"

#include <cstdarg>

// Jolt includes
#include <Jolt/Core/Factory.h>

// All Jolt symbols are in the JPH namespace
using namespace JPH;

namespace undicht {

    namespace physics {

        ////////////////////////////////////// callbacks for jolt physics //////////////////////////////////////

        void traceCallback(const char *inFMT, ...) {
            // Format the message
            va_list list;
            va_start(list, inFMT);
            char buffer[1024];
            vsnprintf(buffer, sizeof(buffer), inFMT, list);
            va_end(list);

            // Print to the TTY
            UND_LOG << buffer << "\n";
        }

        bool assertFailedCallback(const char *inExpression, const char *inMessage, const char *inFile, uint inLine) {
            // Print to the TTY
            UND_ERROR << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr? inMessage : "") << "\n";

            // Breakpoint
            return true;
        }

        ////////////////////////////////////// init / cleanup of jolt physics library //////////////////////////////////////
    
        void initJoltPhysics() {

            // Register allocation hook
            RegisterDefaultAllocator();

            // Install callbacks
            Trace = traceCallback;
            JPH_IF_ENABLE_ASSERTS(AssertFailed = assertFailedCallback;)

            // Create a factory
            Factory::sInstance = new Factory();

            // Register all Jolt physics types
	        RegisterTypes();
        }

        void cleanUpJoltPhysics() {

            // Unregisters all types with the factory and cleans up the default material
            UnregisterTypes();

            // Destroy the factory
            delete Factory::sInstance;
            Factory::sInstance = nullptr;
        }

    } // physics

} // undicht