#include "node.h"
#include "buffer_layout.h"

namespace undicht {

    namespace graphics {

        void Node::init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache) {

            // ubo to hold the model matrix for this node
            _ubo.init(device, allocator, BufferLayout({UND_MAT4F}));
            
            _descriptor_set = descriptor_cache.allocate();
            _descriptor_set.bindUniformBuffer(0, _ubo);
            _descriptor_set.update();

        }

        void Node::cleanUp() {

            _ubo.cleanUp();

            for(Node& n : _child_nodes)
                n.cleanUp();

        }

        Node& Node::addChildNode(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache) {

            _child_nodes.emplace_back(Node());
            _child_nodes.back().init(device, allocator, descriptor_cache);

            return _child_nodes.back();
        }

        std::vector<Node>& Node::getChildNodes() {

            return _child_nodes;
        }

        void Node::clearChildNodes() {

            for(Node& n : _child_nodes)
                n.cleanUp();

            _child_nodes.clear();
        }

        uint32_t Node::getChildNodeCount() const {

            return _child_nodes.size();
        }
            
        Node& Node::getChildNode(uint32_t id) {

            return _child_nodes.at(id);
        }

        void Node::setMeshes(std::vector<uint32_t> meshes) {

            _meshes = meshes;
        }

        const std::vector<uint32_t>& Node::getMeshes() const {

            return _meshes;
        }

        void Node::setModelMatrix(const uint8_t* data, vulkan::TransferBuffer& transfer_buffer) {

            _ubo.uploadData(0, data, transfer_buffer);
        }

        const vulkan::UniformBuffer& Node::getUbo() const {

            return _ubo;
        }

        const vulkan::DescriptorSet& Node::getDescriptorSet() const {

            return _descriptor_set;
        }

    } // graphics

} // undicht 