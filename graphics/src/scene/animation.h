#ifndef ANIMATION_H
#define ANIMATION_H

#include "string"
#include "vector"
#include "node_animation.h"
#include "node.h"
#include "bone.h"

namespace undicht {

    namespace graphics {

        class SceneGroup;

        class Animation {
          
          protected:

            // attributes
            std::string _name;
            std::vector<NodeAnimation> _node_animations;

            double _duration; // in ticks, not seconds!
            double _ticks_per_second;

          public:

            void init();
            void cleanUp();

            /// @brief the name is a unique identifier with with the animation 
            /// can be adressed within its SceneGroup
            void setName(const std::string& name);
            const std::string& getName() const;

            void setDuration(double duration);
            double getDuration() const;

            void setTicksPerSecond(double ticks_per_second);
            double getTicksPerSecond() const;

            /** @param node_name: the node affected by the NodeAnimation
             * there should not be multiple NodeAnimations affecting the same node
             * so if such a NodeAnimation already exists, it is returned instead */
            NodeAnimation& addNodeAnimation(const std::string& node_name);

            /// @return nullptr, in case no node animation affects the requested node
            NodeAnimation* getNodeAnimation(const std::string& node_name);

            /// update all the nodes transformations
            /// @param time the current time in seconds (reference doesnt matter)
            void update(double time, SceneGroup& group);

          protected:


        };

    } // graphics

} // undicht

#endif // ANIMATION_H