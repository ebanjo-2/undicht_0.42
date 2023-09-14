#ifndef NODE_H
#define NODE_H

#include "cstdint"
#include "vector"

namespace undicht {

    namespace graphics {

        class Node {
        
          protected:

            // child nodes
            std::vector<Node> _child_nodes;

            // attributes of the Node
            std::vector<uint32_t> _meshes;

          public:
 
            Node& addChildNode();
            std::vector<Node>& getChildNodes();

            void setMeshes(std::vector<uint32_t> meshes);
            const std::vector<uint32_t>& getMeshes() const;

        };

    } // graphics

} // undicht

#endif // NODE_H