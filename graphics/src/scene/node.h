#ifndef NODE_H
#define NODE_H

#include "cstdint"
#include "vector"
#include "string"

#include "renderer/vulkan/uniform_buffer.h"
#include "renderer/vulkan/transfer_buffer.h"
#include "renderer/vulkan/descriptor_set_cache.h"

namespace undicht {

    namespace graphics {

        class Node {
        
          protected:

            // child nodes
            std::vector<Node> _child_nodes;

            // attributes of the Node
            std::string _name;
            // std::vector<uint32_t> _meshes;
            std::vector<std::string> _meshes;

            vulkan::UniformBuffer _ubo;
            vulkan::DescriptorSet _descriptor_set;

          public:

            void init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache);
            void cleanUp();

            Node& addChildNode(const std::string& node_name, const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache);
            std::vector<Node>& getChildNodes();
            void clearChildNodes();
            uint32_t getChildNodeCount() const;
            // Node& getChildNode(uint32_t id);
            Node* getChildNode(const std::string& node_name);

            //void setMeshes(std::vector<uint32_t> meshes);
            //const std::vector<uint32_t>& getMeshes() const;

            void setName(const std::string& name);
            const std::string& getName() const;

            void setMeshes(const std::vector<std::string>& meshes);
            const std::vector<std::string>& getMeshes() const;

            void setModelMatrix(const uint8_t* data, vulkan::TransferBuffer& transfer_buffer);
            const vulkan::UniformBuffer& getUbo() const;
            const vulkan::DescriptorSet& getDescriptorSet() const;

        };

    } // graphics

} // undicht

#endif // NODE_H