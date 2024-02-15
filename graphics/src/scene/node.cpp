#include "node.h"
#include "buffer_layout.h"
#include "debug.h"
#include "iomanip"
#include "file_tools.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "scene_group.h"

namespace undicht {

    namespace graphics {

        const size_t MAX_BONES_PER_NODE = 100;

        void Node::init() {
            // init without vulkan objects

            _local_transformation = glm::mat4(1.0f);
        } 

        void Node::init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache) {
            
            init();
            _has_vulkan_objects = true;

            // ubo to hold the model matrix for this node
            //  + transformation matrices for the bones
            // i think i should use a storage buffer?
            std::vector<FixedType> types(1 + MAX_BONES_PER_NODE, UND_MAT4F); // model transformation matrix + bone matrices
            _ubo.init(device, allocator, BufferLayout(types));

            _descriptor_set = descriptor_cache.allocate();
            _descriptor_set.bindUniformBuffer(0, _ubo);
            _descriptor_set.update();

        }

        void Node::cleanUp() {

            if(_has_vulkan_objects) {
                _ubo.cleanUp();
            }

            for(Node& n : _child_nodes)
                n.cleanUp();

            _child_nodes.clear();

        }

        Node& Node::addChildNode(const std::string& node_name) {
            // add without vulkan objects   

            // check if a similarly named node exists
            Node* node = getChildNode(node_name);
            if(node) return *node;

            // create a new node
            _child_nodes.emplace_back(Node());
            _child_nodes.back().init();
            _child_nodes.back().setName(node_name);

            return _child_nodes.back();
        }

        Node& Node::addChildNode(const std::string& node_name, const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache) {

            // check if a similarly named node exists
            Node* node = getChildNode(node_name);
            if(node) return *node;

            // create a new node
            _child_nodes.emplace_back(Node());
            _child_nodes.back().init(device, allocator, descriptor_cache);
            _child_nodes.back().setName(node_name);

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

        Node* Node::getChildNode(const std::string& node_name, bool search_recursive) {

            for(Node& n : _child_nodes)
                if(!n.getName().compare(node_name))
                    return &n;

            if(search_recursive) {
                for(Node& c : _child_nodes) {
                    Node* n = c.getChildNode(node_name, search_recursive);
                    if(n) return n;
                }
            }

            return nullptr;
        }

        void Node::setName(const std::string& name) {

            _name = name;
        }

        const std::string& Node::getName() const {

            return _name;
        }

        void Node::setMesh(const std::string& mesh) {

            _mesh = mesh;
        }

        void Node::addMeshes(const std::vector<std::string>& meshes, const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache) {
            
            for(const std::string& mesh : meshes) {

                addChildNode("mesh child " + toStr(_child_nodes.size()), device, allocator, descriptor_cache).setMesh(mesh);
            }

        }   

        const std::string& Node::getMesh() const {

            return _mesh;
        }

        void Node::setLocalTransformation(const glm::mat4& transformation) {
            /// set the transformation of the nodes local coord. system
            /// relative to its parents

            _local_transformation = transformation;
        }

        void Node::updateGlobalTransformation(const glm::mat4& parent_transformation) {
            /// calculates the transformation of the local coord. system rel. to the global coord. system
            /// updates the children nodes as well

            // transformation of parent coord. system from global
            // * transf. of local system from parent system
            // = transformation of local system from global
            _global_transformation = parent_transformation * _local_transformation;

            // updating the child nodes global transformation
            for(Node& n : _child_nodes)
                n.updateGlobalTransformation(_global_transformation);
        }

        std::ostream& operator<< (std::ostream& out, const glm::mat4& print_matrix) {

            for(int row = 0; row < 4; row++) {
                    for(int col = 0; col < 4; col++) {
                        out << std::setprecision(2) << std::setw(8) <<  print_matrix[col][row] << " ";
                    }
                    out << "\n";
            }

            return out;
        }

    
        const glm::mat4& Node::getLocalTransformation() const {

            return _local_transformation;
        }

        const glm::mat4& Node::getGlobalTransformation() const {

            return _global_transformation;
        }

        void Node::updateUniformBuffer(vulkan::TransferBuffer& transfer_buffer, SceneGroup& scene_group) {

            // updating the child nodes uniform buffers
            for(Node& n : _child_nodes)
                n.updateUniformBuffer(transfer_buffer, scene_group);

            // updating this nodes uniform buffer
            if(!_has_vulkan_objects) return;
            Mesh* mesh = scene_group.getMesh(_mesh);
            if(!mesh) return;

            // get the bone matrices from the skeletons
            std::vector<glm::mat4> bone_matrices;
            for(const std::string& bone_name : mesh->getBones()) {
                Bone* b = scene_group.getBone(bone_name);
                if(b)bone_matrices.push_back(b->getBoneMatrix());
                else UND_ERROR << "failed to update bone: " << bone_name << "\n";
            }

            // UND_LOG << "updating " << bone_matrices.size() << " bone matrices\n";

            if(bone_matrices.size() > MAX_BONES_PER_NODE)
                UND_WARNING << "provided number of Bone Matrices cant be stored, expect animation glitches\n";

            // upload the model matrix to the ubo
            _ubo.uploadData(0, (const uint8_t*)glm::value_ptr(_global_transformation), transfer_buffer);

            // upload the bone matrices
            for(int i = 0; i < std::min(bone_matrices.size(), MAX_BONES_PER_NODE); i++)
                _ubo.uploadData(i + 1, (const uint8_t*)glm::value_ptr(bone_matrices[i]), transfer_buffer);

        }

        const vulkan::UniformBuffer& Node::getUbo() const {

            return _ubo;
        }

        const vulkan::DescriptorSet& Node::getDescriptorSet() const {

            return _descriptor_set;
        }

        bool Node::getHasVulkanObjects() const {

            return _has_vulkan_objects;
        }

    } // graphics

} // undicht 