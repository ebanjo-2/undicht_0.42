#ifndef OBJECT_LAYERS_H
#define OBJECT_LAYERS_H

#include "physics.h"
#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace undicht {

    namespace physics {

        // Layer that objects can be in, determines which other objects it can collide with
        // Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
        // layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
        // but only if you do collision testing).
        namespace layers {
            constexpr JPH::ObjectLayer NON_MOVING = 0;
            constexpr JPH::ObjectLayer MOVING = 1;
            constexpr JPH::ObjectLayer NUM_LAYERS = 2;
        };

        /// Class that determines if two object layers can collide
        class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {

          public:

            virtual bool ShouldCollide(JPH::ObjectLayer layer_1, JPH::ObjectLayer layer_2) const override;

        };

        // Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
        // a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
        // You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
        // many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
        // your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
        namespace broad_phase_layers {
            constexpr JPH::BroadPhaseLayer NON_MOVING(0);
            constexpr JPH::BroadPhaseLayer MOVING(1);
            constexpr uint NUM_LAYERS = 2;
        };

        // BroadPhaseLayerInterface implementation
        // This defines a mapping between object and broadphase layers.
        class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
          private:

            // 1-on-1 mapping between object layers and broadphase layers
            JPH::BroadPhaseLayer _object_to_broad_phase[layers::NUM_LAYERS];

          public:

            BPLayerInterfaceImpl();

            virtual uint GetNumBroadPhaseLayers() const override;
            virtual JPH::BroadPhaseLayer	GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
            virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override;
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

        };

        /// Class that determines if an object layer can collide with a broadphase layer
        class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {

          public:

            virtual bool ShouldCollide(JPH::ObjectLayer layer_1, JPH::BroadPhaseLayer layer_2) const override;
           
        };

    } // physics

} // undicht

#endif // OBJECT_LAYERS_H