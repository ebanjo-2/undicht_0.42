#ifndef SCENE_H
#define SCENE_H

#include "vulkan_memory_allocator.h"
#include "core/vulkan/logical_device.h"

#include "mesh.h"

#include "vector"

namespace undicht {

    class Scene {
        /// implementing a scene structure similar to the one described 
        /// by https://learnopengl.com/Model-Loading/Assimp
        /// (assimps scene structure)
      protected:

        vulkan::LogicalDevice _device_handle;
        vma::VulkanMemoryAllocator _allocator_handle;
        
        std::vector<Mesh> _meshes;

      public:

        void init(const vulkan::LogicalDevice& device, vma::VulkanMemoryAllocator& allocator);
        void cleanUp();
        
        Mesh& addMesh();


    };

} // undicht

#endif // SCENE_H