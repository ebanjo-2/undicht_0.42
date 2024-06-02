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

        class SceneGroup; // node.h gets included by scene_group.h

        class Node {
        
          protected:

            // node hierarchy
            std::vector<Node> _child_nodes;

            // attributes of the Node
            std::string _name;
            std::string _mesh; // one mesh per node
            uint32_t _mesh_id = 0xFFFFFFFF; // a faster way to access the mesh from the scene
            glm::mat4 _local_transformation; // transformation of local coord. system rel. to parent
            glm::mat4 _global_transformation; // transformation of local coord. system rel. to global system

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
            Mesh* getMesh(SceneGroup& scene);

            /// set the transformation of the nodes local coord. system
            /// relative to its parents
            void setLocalTransformation(const glm::mat4& transformation);
            /// calculates the transformation of the local coord. system rel. to the global coord. system
            /// updates the children nodes as well
            void updateGlobalTransformation(const glm::mat4& parent_transformation);

            const glm::mat4& getLocalTransformation() const;
            const glm::mat4& getGlobalTransformation() const;
            
            void updateUniformBuffer(vulkan::TransferBuffer& transfer_buffer, SceneGroup& scene_group);
            
            const vulkan::UniformBuffer& getUbo() const;
            const vulkan::DescriptorSet& getDescriptorSet() const;
            bool getHasVulkanObjects() const;

        };

    } // graphics

} // undicht

#endif // NODE_H