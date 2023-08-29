#include "node.h"

namespace undicht {

    Node& Node::addChildNode() {

        _child_nodes.emplace_back(Node());

        return _child_nodes.back();
    }

    std::vector<Node>& Node::getChildNodes() {

        return _child_nodes;
    }

    void Node::setMeshes(std::vector<uint32_t> meshes) {

        _meshes = meshes;
    }

    const std::vector<uint32_t>& Node::getMeshes() const {

        return _meshes;
    }

} // undicht 