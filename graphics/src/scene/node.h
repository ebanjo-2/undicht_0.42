#ifndef NODE_H
#define NODE_H

#include "cstdint"
#include "vector"
#include "string"

#include "renderer/vulkan/uniform_buffer.h"
#include "renderer/vulkan/transfer_buffer.h"
#include "renderer/vulkan/descriptor_set_cache.h"
#include "glm/glm.hpp"

#include "mesh.h"

namespace undicht {

    namespace graphics {

        class Node {
        
          protected:

            // node hierarchy
            std::vector<Node> _child_nodes;

            // attributes of the Node
            std::string _name;
            std::string _mesh; // one mesh per node
            glm::mat4 _local_transformation; // transformation from local to parent coordinate system
            glm::mat4 _parent_transformation; // transformation from parent to global coordinate system
            glm::mat4 _global_transformation; // transformation from local to global coordinate system
            std::vector<glm::mat4> _bone_matrices;

            // vulkan objects
            bool _has_vulkan_objects = false;
            vulkan::UniformBuffer _ubo;
            vulkan::DescriptorSet _descriptor_set;

          public:

            void init(); // init without vulkan objects
            void init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache);
            void cleanUp();

            Node& addChildNode(const std::string& node_name); // add without vulkan objects
            Node& addChildNode(const std::string& node_name, const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache);
            std::vector<Node>& getChildNodes();
            void clearChildNodes();
            uint32_t getChildNodeCount() const;
            Node* getChildNode(const std::string& node_name, bool search_recursive = false);

            void setName(const std::string& name);
            const std::string& getName() const;

            void setMesh(const std::string& mesh);
            // adds the meshes as child nodes
            void addMeshes(const std::vector<std::string>& meshes, const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache);
            const std::string& getMesh() const;

            /// set the transformation from the local coordinate system to the parents
            /// also updates the global transformation (including its children)
            void setLocalTransformation(const glm::mat4& transformation);
            /// updates the transformation from local space to global space
            /// recursivly calls the function for all its children
            void updateGlobalTransformation(const glm::mat4& parent_transformation);

            /// updates the bone transformation matrices to accuratly represent their current position
            /// only one mesh with animation allowed per node
            /// the skeletal_hierarchy (all bones used in the animation) must be children of this node
            /// @param meshes must contain the mesh used by the node for animation
            void updateBoneMatrices(Node& skeletal_hierarchy, const std::vector<Mesh>& meshes);

            const glm::mat4& getLocalTransformation() const;
            const glm::mat4& getGlobalTransformation() const;
            
            void updateUniformBuffer(vulkan::TransferBuffer& transfer_buffer);
            
            const vulkan::UniformBuffer& getUbo() const;
            const vulkan::DescriptorSet& getDescriptorSet() const;
            bool getHasVulkanObjects() const;

        };

    } // graphics

} // undicht

#endif // NODE_H