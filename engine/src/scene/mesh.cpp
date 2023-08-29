#include "mesh.h"

#include "debug.h"

#include "core/vulkan/command_buffer.h"
#include "core/vulkan/fence.h"

namespace undicht {

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

    void Mesh::setVertexData(const char* data, uint32_t byte_size) {

        // init the vertex buffer
        VkBufferUsageFlags usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        VmaAllocationCreateFlags vma_flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        _vertex_buffer.init(_allocator_handle, {_device_handle.getGraphicsQueueFamily()}, byte_size, usage_flags, vma_flags);

        // upload data to the vertex buffer
        uploadToBuffer(_vertex_buffer, data, byte_size);

    }

    void Mesh::setIndexData(const char* data, uint32_t byte_size) {

        // init the face buffer
        VkBufferUsageFlags usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        VmaAllocationCreateFlags vma_flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        _index_buffer.init(_allocator_handle, {_device_handle.getGraphicsQueueFamily()}, byte_size, usage_flags, vma_flags);

        // upload data to the vertex buffer
        uploadToBuffer(_index_buffer, data, byte_size);
    }

    void Mesh::setVertexAttributes(bool has_positions, bool has_tex_coords, bool has_normals, bool has_tangents_bitangents) {

        _has_positions = has_positions;
        _has_tex_coords = has_tex_coords;
        _has_normals = has_normals;
        _has_tangents_and_bitangents = has_tangents_bitangents;
    }

    void Mesh::setMaterialID(uint32_t material_id) {

        _material_id = material_id;
    }

    void Mesh::setName(const std::string& name) {

        _name = name;
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

    uint32_t Mesh::getMaterialID() const {

        return _material_id;
    }

    std::string Mesh::getName() const {

        return _name;
    }

    ///////////////////////////// non public mesh functions /////////////////////////////

    void Mesh::uploadToBuffer(Buffer& dst, const char* data, uint32_t byte_size) {
        // using a staging buffer to store data in a buffer that is only visible to the gpu

        // init the staging buffer
        Buffer staging_buffer;
        VkBufferUsageFlags tmp_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        VmaAllocationCreateFlags tmp_vma_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        staging_buffer.init(_allocator_handle, {_device_handle.getGraphicsQueueFamily()}, byte_size, tmp_usage_flags, tmp_vma_flags);

        // store data in staging buffer
        staging_buffer.setData(byte_size, 0, data);

        // copy data from staging buffer to vertex buffer
        VkBufferCopy copy_info = Buffer::createBufferCopy(byte_size, 0, 0);
        CommandBuffer cmd;
        cmd.init(_device_handle.getDevice(), _device_handle.getGraphicsCmdPool());
        cmd.beginCommandBuffer(true);
        cmd.copy(staging_buffer.getBuffer(), _vertex_buffer.getBuffer(), copy_info);
        cmd.endCommandBuffer();

        // set up fence as a way to know when the copy operation has finished
        Fence copy_finished;
        copy_finished.init(_device_handle.getDevice(), false);

        // submit copy cmd + clean up
        _device_handle.submitOnGraphicsQueue(cmd.getCommandBuffer(), copy_finished.getFence());
        copy_finished.waitForProcessToFinish();
        copy_finished.cleanUp();
        cmd.cleanUp();
        staging_buffer.cleanUp();

    }

} // undicht