#ifndef BUFFER_H
#define BUFFER_H

#include "vulkan/vulkan.h"
#include "vector"
#include "logical_device.h"
#include "vulkan_memory_allocator.h"

namespace undicht {

    namespace vulkan {

        class Buffer {
            /** "Buffers represent linear arrays of data which are used for various purposes by binding them to a graphics or compute pipeline 
             * via descriptor sets or certain commands, or by directly specifying them as parameters to certain commands"
             * (https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkBuffer.html)*/


          protected:

			      VmaAllocator _allocator_handle;

            VkBuffer _buffer;
            VmaAllocation _allocation;
            VmaAllocationInfo _allocation_info;
            VkMemoryPropertyFlags _mem_prop_flags; // contains whether or not the buffer is host visible

          public:
            
            /** @param queue_ids the ids of the queue families from which the buffer is going to be used 
             * @param byte_size the size of memory that will be allocated for the buffer 
             * @param alloc_flags follow the guide by amd: https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/usage_patterns.html 
             * for gpu only resources (like vertex buffers) : use VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
             * for staging buffers (copy memory to gpu only resource) : VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT
             * for readback buffer (copy memory from gpu to cpu visible memory) : VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT 
             * if you want to use the setData() and getData() functions you need to have the VMA_ALLOCATION_CREATE_MAPPED_BIT flags set */
            void init(vma::VulkanMemoryAllocator& allocator, const std::vector<uint32_t>& queue_ids, uint32_t byte_size, VkBufferUsageFlags buffer_usage, VmaAllocationCreateFlags alloc_flags);
            void cleanUp();

            /** @return whether or not you can directly store / read data from the buffer */
            bool isCPUVisible() const;

            uint32_t getAllocatedSize() const;

            /** @brief reads a chunk of data from the buffer
             * make sure that there is enough memory allocated both in the buffer and data 
             * will only work if the buffer is initialized with the VMA_ALLOCATION_CREATE_MAPPED_BIT flag */
            void setData(uint32_t byte_size, uint32_t offset, const void* data);

            /** @brief reads a chunk of data from the buffer
             * make sure that there is enough memory allocated both in the buffer and data 
             * will only work if the buffer is initialized with the VMA_ALLOCATION_CREATE_MAPPED_BIT flag */
            void readData(uint32_t byte_size, uint32_t offset, void* data);

          protected:
            // creating buffer related structs

            VkBufferCreateInfo static createBufferCreateInfo(uint32_t byte_size, VkBufferUsageFlags usage, VkBufferCreateFlags flags, const std::vector<uint32_t>& queue_ids);
			      VmaAllocationCreateInfo static createVmaAllocationCreateInfo(VmaMemoryUsage usage, VmaAllocationCreateFlags flags);

          public:

            VkBufferCopy static createBufferCopy(uint32_t byte_size, uint32_t src_offset, uint32_t dst_offset);

        };

    } // vulkan

} // undicht

#endif // BUFFER_H