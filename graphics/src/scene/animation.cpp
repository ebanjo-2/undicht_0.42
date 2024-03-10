#include "animation.h"
#include "debug.h"
#include "glm/glm.hpp"
#include "scene_group.h"

namespace undicht {

    namespace graphics {

        void Animation::init() {

        }

        void Animation::cleanUp() {

        }

        void Animation::setName(const std::string& name) {
            /// @brief the name is a unique identifier with with the animation 
            /// can be adressed within its SceneGroup

            _name = name;
        }

        const std::string& Animation::getName() const {

            return _name;
        }

        void Animation::setDuration(double duration) {

            _duration = duration;
        }

        double Animation::getDuration() const {

            return _duration;
        }

        void Animation::setTicksPerSecond(double ticks_per_second) {

            _ticks_per_second = ticks_per_second;
        }

        double Animation::getTicksPerSecond() const {

            return _ticks_per_second;
        }

        NodeAnimation& Animation::addNodeAnimation(const std::string& node_name) {
            /** @param node_name: the node affected by the NodeAnimation
             * there should not be multiple NodeAnimations affecting the same node
             * so if such a NodeAnimation already exists, it is returned instead of a new one*/

            NodeAnimation* anim = getNodeAnimation(node_name);
            if(anim) return *anim;

            _node_animations.emplace_back(NodeAnimation());
            _node_animations.back().setNode(node_name);

            return _node_animations.back();
        }

        NodeAnimation* Animation::getNodeAnimation(const std::string& node_name) {
            /// @return nullptr, in case no node animation affects the requested node

            for(NodeAnimation& n : _node_animations)
                if(!n.getNode().compare(node_name))
                    return &n;

            return nullptr;
        }
        
        void Animation::update(double time, SceneGroup& group) {
            // update all the nodes transformations 
            /// @param time the current time in seconds (reference doesnt matter)

            double start_time = 0.0;
            double duration_in_seconds = _duration / _ticks_per_second;
            double rel_progress = (time - start_time) / duration_in_seconds;
            double repeating_progress = rel_progress - uint64_t(rel_progress);

            // time for the animation
            double rel_time = repeating_progress * _duration;

            for(NodeAnimation& node_anim : _node_animations) {
                
                glm::mat4 transf = node_anim.getTransfMat(rel_time);
                Bone* bone = group.getBone(node_anim.getNode());

                if(!bone) {
                    UND_ERROR << "Bone " << node_anim.getNode() << " couldnt be updated\n";
                    continue;
                }

                //UND_LOG << "updated bone: " << node_anim.getNode() << "\n";

                // replace the current transformation mat for the bone
                bone->setLocalMatrix(transf);
            }

        }

    } // graphics

} // undich