#ifndef NODE_ANIMATION_H
#define NODE_ANIMATION_H

#include "string"

namespace undicht {

    namespace graphics {

        class NodeAnimation {

          protected:

            std::string _node;

          public:

            void setNode(const std::string& node);
            const std::string& getNode() const;

        };

    } // graphics

} // undicht

#endif // NODE_ANIMATION_H