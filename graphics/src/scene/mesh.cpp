#include "mesh.h"

#include "debug.h"

#include "core/vulkan/command_buffer.h"
#include "core/vulkan/fence.h"

namespace undicht {

    namespace graphics {

        using namespace vulkan;
        using namespace vma;

        void Mesh::init(const LogicalDevice& device, VulkanMemoryAllocator& allocator) {

            _device_handle = device;
            _allocator_handle = allocator;
        }

        void Mesh::cleanUp() {

            _vertex_buffer.cleanUp();
            _index_buffer.cleanUp();
        }

        void Mesh::setVertexData(const uint8_t* data, uint32_t byte_size, TransferBuffer& transfer_buffer) {

            // init the vertex buffer
            VkBufferUsageFlags usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            VmaAllocationCreateFlags vma_flags = {};
            _vertex_buffer.init(_allocator_handle, {_device_handle.getGraphicsQueueFamily()}, byte_size, usage_flags, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, vma_flags);

            // upload data to the vertex buffer
            transfer_buffer.stageForTransfer(_vertex_buffer.getBuffer(), byte_size, 0, data);

        }

        void Mesh::setIndexData(const uint8_t* data, uint32_t byte_size, TransferBuffer& transfer_buffer) {

            // init the face buffer
            VkBufferUsageFlags usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            VmaAllocationCreateFlags vma_flags = {};
            _index_buffer.init(_allocator_handle, {_device_handle.getGraphicsQueueFamily()}, byte_size, usage_flags, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, vma_flags);

            // upload data to the vertex buffer
            transfer_buffer.stageForTransfer(_index_buffer.getBuffer(), byte_size, 0, data);

        }

        void Mesh::setVertexCount(uint32_t count) {

            _vertex_count = count;
        }

        void Mesh::setVertexAttributes(bool has_positions, bool has_tex_coords, bool has_normals, bool has_tangents_bitangents, bool has_bones) {

            _has_positions = has_positions;
            _has_tex_coords = has_tex_coords;
            _has_normals = has_normals;
            _has_tangents_and_bitangents = has_tangents_bitangents;
            _has_bones = has_bones;
        }

        /*void Mesh::setMaterialID(uint32_t material_id) {

            _material_id = material_id;
        }*/

        void Mesh::setName(const std::string& name) {

            _name = name;
        }

        void Mesh::setMaterial(const std::string& material) {

            _material = material;
        }

        bool Mesh::getHasPositions() const {
            
            return _has_positions;
        }

        bool Mesh::getHasTexCoords() const {

            return _has_tex_coords;
        }

        bool Mesh::getHasNormals() const {

            return _has_normals;
        }

        bool Mesh::getHasTangentsBitangents() const {

            return _has_tangents_and_bitangents;
        }

        bool Mesh::getHasBones() const {

            return _has_bones;
        }

        uint32_t Mesh::getVertexCount() const {

            return _vertex_count;
        }

        /*uint32_t Mesh::getMaterialID() const {

            return _material_id;
        }*/

        const std::string& Mesh::getName() const {

            return _name;
        }

        const std::string& Mesh::getMaterial() const {

            return _material;
        }

        const vulkan::Buffer& Mesh::getVertexBuffer() const {

            return _vertex_buffer;
        }

        const vulkan::Buffer& Mesh::getIndexBuffer() const {

            return _index_buffer;
        }

    } // graphics

} // undicht