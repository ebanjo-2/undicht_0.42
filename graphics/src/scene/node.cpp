#include "node.h"
#include "buffer_layout.h"
#include "debug.h"
#include "iomanip"
#include "file_tools.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace undicht {

    namespace graphics {

        const size_t MAX_BONES_PER_NODE = 100;

        void Node::init() {
            // init without vulkan objects

            _local_transformation = glm::mat4(1.0f);
            _parent_transformation = glm::mat4(1.0f);
        } 

        void Node::init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, vulkan::DescriptorSetCache& descriptor_cache) {
            
            init();
            _has_vulkan_objects = true;

            // ubo to hold the model matrix for this node
            // (transformation from the model to the global coordinate system)
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
            /// set the transformation from the local coordinate system to the parents
            /// also updates the global transformation (including its children)

            _local_transformation = transformation;

            updateGlobalTransformation(_parent_transformation);
        }

        void Node::updateGlobalTransformation(const glm::mat4& parent_transformation) {
            /// updates the transformation from local space to global space
            /// recursivly calls the function for all its children

            _parent_transformation = parent_transformation;
            //_global_transformation = _local_transformation * parent_transformation;
            _global_transformation = parent_transformation * _local_transformation; // this one *should* be it

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

        void Node::updateBoneMatrices(Node& skeletal_hierarchy, const std::vector<Mesh>& meshes) {
            /// updates the bone transformation matrices to accuratly represent their current position
            /// only one mesh with animation allowed per node
            /// the skeletal_hierarchy (all bones used in the animation) must be children of this node
            /// @param meshes must contain the mesh used by the node for animation

            // retrieving the mesh used in the animation
            const Mesh* mesh = nullptr;

            if(_mesh.length()) {
                for(const Mesh& m : meshes) {
                    if(!m.getName().compare(_mesh)) {
                        mesh = &m;
                        break;
                    }   
                }
            }


            if(mesh && mesh->getHasBones()) {

                UND_LOG << "updating the bone matrices for mesh " << _mesh << "\n";

                // updating the matrices for each bone used by the mesh
                _bone_matrices.resize(mesh->getBones().size());

               /*const MeshBone& bone = mesh->getBone(0);
                const Node* bone_node = skeletal_hierarchy.getChildNode(bone.getName(), true);

                if(!bone_node) {
                    UND_ERROR << "bone node " << bone.getName() << " was not found\n";
                    return;
                }

                // bind_pose_mesh_to_bone
                // current_bone_to_global

                _bone_matrices[0] = bone_node->getLocalTransformation();
                //_bone_matrices[0] = bone.getOffsetMatrix() * bone_node->getGlobalTransformation() ;
                //_bone_matrices[0] = bone_node->getGlobalTransformation() * glm::inverse(bone_node->getGlobalTransformation());

                UND_LOG << "processing bone: " << bone.getName() << "\n";
                UND_LOG << "bone to global: " << "\n";

                //glm::mat4 print_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, -10.0f));

                glm::mat4 print_matrix = _bone_matrices[0];
                //glm::mat4 print_matrix = bone_node->getLocalTransformation();
                //glm::mat4 print_matrix = bone_node->getGlobalTransformation();
                //glm::mat4 print_matrix = bone.getOffsetMatrix();
                //glm::mat4 print_matrix = global_to_node;


                */

                for(int i = 0; i < mesh->getBones().size(); i++) {

                    const MeshBone& bone = mesh->getBone(i);
                    const Node* bone_node = skeletal_hierarchy.getChildNode(bone.getName(), true);
                    
                    if(bone_node) {

                        // bone_to_mesh * mesh_to_bind_pose = bone_to_bind_pose
                        //_bone_matrices[i] = bone_node->getLocalTransformation();
                        _bone_matrices[i] = bone_node->getGlobalTransformation() * bone.getOffsetMatrix(); // this one *should* be it
                        //_bone_matrices[i] = bone.getOffsetMatrix() * bone_node->getGlobalTransformation();

                        UND_LOG << "bone " << bone.getName() << "\n";
                        std::cout << _bone_matrices[i] << "\n";

                    } else {
                        // no bone found, using the identity matrix
                        UND_ERROR << "failed to find bone node: " << bone.getName() << "\n";
                        _bone_matrices[i] = glm::mat4(1.0f);
                    }
                    //_bone_matrices[i] = glm::mat4(0.0f);

                }

            }

            // recursivly updating the child nodes as well
            for(Node& n : _child_nodes)
                n.updateBoneMatrices(skeletal_hierarchy, meshes);

        }

        const glm::mat4& Node::getLocalTransformation() const {

            return _local_transformation;
        }

        const glm::mat4& Node::getGlobalTransformation() const {

            return _global_transformation;
        }

        void Node::updateUniformBuffer(vulkan::TransferBuffer& transfer_buffer) {

            if(_bone_matrices.size() > MAX_BONES_PER_NODE)
                UND_WARNING << "provided number of Bone Matrices cant be stored, expect animation glitches\n";

            if(_has_vulkan_objects) {
            
                // upload the model matrix to the ubo
                _ubo.uploadData(0, (const uint8_t*)glm::value_ptr(_global_transformation), transfer_buffer);

                // upload the bone matrices
                for(int i = 0; i < std::min(_bone_matrices.size(), MAX_BONES_PER_NODE); i++)
                    _ubo.uploadData(i + 1, (const uint8_t*)glm::value_ptr(_bone_matrices[i]), transfer_buffer);

            }

            // updating the child nodes uniform buffers
            for(Node& n : _child_nodes)
                n.updateUniformBuffer(transfer_buffer);
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