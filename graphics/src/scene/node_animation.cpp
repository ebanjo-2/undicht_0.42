#include "node_animation.h"

namespace undicht {

    namespace graphics {

        void NodeAnimation::setNode(const std::string& node) {

            _node = node;
        }

        const std::string& NodeAnimation::getNode() const {

            return _node;
        }

    } // graphics

} // undicht