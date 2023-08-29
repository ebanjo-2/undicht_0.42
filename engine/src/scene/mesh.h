#ifndef MESH_H
#define MESH_H

#include "vulkan_memory_allocator.h"
#include "core/vulkan/buffer.h"
#include "core/vulkan/logical_device.h"

#include "string"

namespace undicht {

    class Mesh {
        /** contains vertices, which can have such attributes as a position or texture coordinate
         * and it contains faces, which define the way to create a mesh from the vertices
         * if they are present, the vertex attributes are ALWAYS ordered in the following way:
         * Vec3f position
         * Vec3f texture coordinate
         * Vec3f normal
         * Vec3f tangent
         * Vec3f bitangent */
      protected:

        vulkan::LogicalDevice _device_handle;
        vma::VulkanMemoryAllocator _allocator_handle;

        // vulkan objects that contain the mesh data
        vulkan::Buffer _vertex_buffer;
        vulkan::Buffer _index_buffer;

        // vertex attributes
        bool _has_positions;
        bool _has_tex_coords;
        bool _has_normals;
        bool _has_tangents_and_bitangents; // if there is one there is also the other

        // other attributes
        uint32_t _material_id;
        std::string _name;

      public:

        void init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator);
        void cleanUp();

        void setVertexData(const char* data, uint32_t byte_size);
        void setIndexData(const char* data, uint32_t byte_size);

        void setVertexAttributes(bool has_positions, bool has_tex_coords, bool has_normals, bool has_tangents_bitangents);
        void setMaterialID(uint32_t material_id);
        void setName(const std::string& name);

        bool getHasPositions() const;
        bool getHasTexCoords() const;
        bool getHasNormals() const;
        bool getHasTangentsBitangents() const;
        uint32_t getMaterialID() const;
        std::string getName() const;

      protected:
        // non public mesh functions

        void uploadToBuffer(vulkan::Buffer& dst, const char* data, uint32_t byte_size);

    };

} // undicht

#endif // MESH_H