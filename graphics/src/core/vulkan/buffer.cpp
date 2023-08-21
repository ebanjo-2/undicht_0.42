#include "buffer.h"

#include "cassert"
#include "cstring"

namespace undicht {

    namespace vulkan {

        void Buffer::init(vma::VulkanMemoryAllocator& allocator, const std::vector<uint32_t>& queue_ids, uint32_t byte_size, VkBufferUsageFlags buffer_usage, VmaAllocationCreateFlags alloc_flags) {
            /** @param queue_ids the ids of the queue families from which the buffer is going to be used 
             * @param byte_size the size of memory that will be allocated for the buffer 
             * @param alloc_flags follow the guide by amd: https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/usage_patterns.html 
             * for gpu only resources (like vertex buffers) : use VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
             * for staging buffers (copy memory to gpu only resource) : VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT
             * for readback buffer (copy memory from gpu to cpu visible memory) : VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT 
             * if you want to use the setData() and getData() functions you need to have the VMA_ALLOCATION_CREATE_MAPPED_BIT flags set */

           _allocator_handle = allocator.getVmaAllocator();

            // choosing buffer creation parameters depending on the intended usage
            VkBufferCreateInfo buffer_info;
            VmaAllocationCreateInfo alloc_info;

            buffer_info = createBufferCreateInfo(byte_size, buffer_usage, {}, queue_ids);
            alloc_info = createVmaAllocationCreateInfo(VMA_MEMORY_USAGE_AUTO, alloc_flags);
            
            vmaCreateBuffer(allocator.getVmaAllocator(), &buffer_info, &alloc_info, &_buffer, &_allocation, &_allocation_info);
            vmaGetAllocationMemoryProperties(allocator.getVmaAllocator(), _allocation, &_mem_prop_flags);
        }

        void Buffer::cleanUp() {

            vmaDestroyBuffer(_allocator_handle, _buffer, _allocation);
        }

        bool Buffer::isCPUVisible() const {
            /** @return whether or not you can directly store / read data from the buffer */

            return _mem_prop_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        }

        uint32_t Buffer::getAllocatedSize() const {

            return _allocation_info.size;
        }

        void Buffer::setData(uint32_t byte_size, uint32_t offset, const void* data) {
            /** @brief reads a chunk of data from the buffer
             * make sure that there is enough memory allocated both in the buffer and data 
             * will only work if the buffer is initialized with the VMA_ALLOCATION_CREATE_MAPPED_BIT flag */
            
            assert(_allocation_info.pMappedData);

            memcpy(_allocation_info.pMappedData + offset, data, byte_size);
        }


        void Buffer::readData(uint32_t byte_size, uint32_t offset, void* data) {
            /** @brief reads a chunk of data from the buffer
             * make sure that there is enough memory allocated both in the buffer and data 
             * will only work if the buffer is initialized with the VMA_ALLOCATION_CREATE_MAPPED_BIT flag */

            assert(_allocation_info.pMappedData);

            memcpy(data, _allocation_info.pMappedData + offset, byte_size);
        }

        //////////////////////////////////// creating Buffer related structs ///////////////////////////////////

        VkBufferCreateInfo Buffer::createBufferCreateInfo(uint32_t byte_size, VkBufferUsageFlags usage, VkBufferCreateFlags flags, const std::vector<uint32_t>& queue_ids) {

            VkBufferCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            info.pNext = nullptr;
            info.usage = usage;
            info.queueFamilyIndexCount = queue_ids.size();
            info.pQueueFamilyIndices = queue_ids.data();
            info.sharingMode = queue_ids.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
            info.size = byte_size;
            info.flags = flags;

            return info;
        }

        VmaAllocationCreateInfo Buffer::createVmaAllocationCreateInfo(VmaMemoryUsage usage, VmaAllocationCreateFlags flags) {

            VmaAllocationCreateInfo info{};
            info.usage = usage;
            info.flags = flags;

            return info;
        }

        VkBufferCopy Buffer::createBufferCopy(uint32_t byte_size, uint32_t src_offset, uint32_t dst_offset) {
            
            VkBufferCopy copy_info{};
            copy_info.size = byte_size;
            copy_info.srcOffset = src_offset;
            copy_info.dstOffset = dst_offset;

            return copy_info;
        }

    } // vulkan

} // undicht