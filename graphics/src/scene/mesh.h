#ifndef MESH_H
#define MESH_H

#include "vulkan_memory_allocator.h"
#include "core/vulkan/buffer.h"
#include "core/vulkan/logical_device.h"
#include "renderer/vulkan/transfer_buffer.h"

#include "string"

namespace undicht {

    namespace graphics {

        class Material;
        class SceneGroup;

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
            bool _has_bones; // for skeletal animations

            uint32_t _vertex_count;

            // other attributes
            std::string _name;
            std::string _material;
            uint32_t _material_id = 0xFFFFFFFF; // an id, with which the material can be accessed faster
            std::vector<std::string> _bones;

          public:

            void init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator);
            void cleanUp();

            void setVertexData(const uint8_t* data, uint32_t byte_size, vulkan::TransferBuffer& transfer_buffer);
            void setIndexData(const uint8_t* data, uint32_t byte_size, vulkan::TransferBuffer& transfer_buffer);

            void setVertexCount(uint32_t count);
            void setVertexAttributes(bool has_positions, bool has_tex_coords, bool has_normals, bool has_tangents_bitangents, bool has_bones);
            void setMaterial(const std::string& material);
            void setName(const std::string& name);
            void setBones(const std::vector<std::string>& bones);

            bool getHasPositions() const;
            bool getHasTexCoords() const;
            bool getHasNormals() const;
            bool getHasTangentsBitangents() const;
            bool getHasBones() const;
            uint32_t getVertexCount() const;
            const std::string& getName() const;
            Material* getMaterial(SceneGroup& scene);
            const std::vector<std::string>& getBones() const;

            /// @return -1, if no bone with the name was found
            int getBoneID(const std::string& bone_name) const;
            const std::string& getBone(int bone_id) const;

            const vulkan::Buffer& getVertexBuffer() const;
            const vulkan::Buffer& getIndexBuffer() const;

        };

    } // graphics

} // undicht

#endif // MESH_H