#include "object_layers.h"

using namespace JPH;

namespace undicht {

    namespace physics {

        ////////////////////////////////// defining object layers ////////////////////////////

        bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer layer_1, JPH::ObjectLayer layer_2) const {

            // at least one has to be moving for a collision to be possible
            return (layer_1 == layers::MOVING) || (layer_2 == layers::MOVING);
        }

        /////////////// defining broadphase layers and mapping object layers to them //////////////

        BPLayerInterfaceImpl::BPLayerInterfaceImpl() {
		    // Create a mapping table from object to broad phase layer
		    _object_to_broad_phase[layers::NON_MOVING] = broad_phase_layers::NON_MOVING;
		    _object_to_broad_phase[layers::MOVING] = broad_phase_layers::MOVING;
	    }

        uint BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const  {
            return broad_phase_layers::NUM_LAYERS;
        }

        BroadPhaseLayer	BPLayerInterfaceImpl::GetBroadPhaseLayer(ObjectLayer layer) const {
            JPH_ASSERT(layer < layers::NUM_LAYERS);
            return _object_to_broad_phase[layer];
        }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        const char* BPLayerInterfaceImpl::GetBroadPhaseLayerName(BroadPhaseLayer layer) const {
            switch ((BroadPhaseLayer::Type)layer){
              case (BroadPhaseLayer::Type)broad_phase_layers::NON_MOVING : 
                return "NON_MOVING";
              case (BroadPhaseLayer::Type)broad_phase_layers::MOVING:		
                return "MOVING";
              default:													
                JPH_ASSERT(false); return "INVALID";
            }
        }
#endif

        ///////////////// define if an object layer can collide with an broadphase layer /////////////
        
        bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(ObjectLayer layer_1, BroadPhaseLayer layer_2) const {

            // at least one has to be moving for a collision to be possible
            return (layer_1 == layers::MOVING) || (layer_2 == broad_phase_layers::MOVING);
        }

    } // physics

} // undicht