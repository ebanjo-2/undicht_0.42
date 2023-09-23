#include "uniform_buffer.h"
#include "debug.h"
#include "core/vulkan/fence.h"
#include "core/vulkan/command_buffer.h"

namespace undicht {

    namespace vulkan {

        void UniformBuffer::init(const LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, const BufferLayout& layout) {

            _layout = layout;
            _allocator_handle = allocator;
            _device_handle = device;

            // calculating the correct allignment for the attributes
            _offsets = calcOffsets(layout);
            _buffer_size = _offsets.back() + layout.m_types.back().getSize();

            // initializing the buffer
            VkBufferUsageFlags buffer_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            VmaAllocationCreateFlags memory_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
            Buffer::init(allocator, {device.getGraphicsQueueFamily()}, _buffer_size, buffer_flags, VMA_MEMORY_USAGE_GPU_ONLY, memory_flags);

        }

        void UniformBuffer::cleanUp() {
            
            Buffer::cleanUp();
        }

        bool UniformBuffer::setData(uint32_t index, const uint8_t* data) {
            /// @brief will try to directly copy the data to the uniform buffer
            /// this will only be possible if the memory is cpu visible, which it may not be depending on the graphics card that is used
            /// @return will return false if its not possible to directly copy the data, in which case you need to use uploadData()  
            
            if(!isCPUVisible()) return false;

            Buffer::setData(_layout.getType(index).getSize(), _offsets.at(index), data);

            return true;
        }

        void UniformBuffer::uploadData(uint32_t index, const uint8_t* data, TransferBuffer& transfer_buffer) {
            /// @brief stores the data in the transfer buffer (needs to have enough memory allocated)
            /// for the data to actually be copied to the uniform buffer, 
            /// you need to call completeTransfers() on the transfer buffer and also submit the command buffer to a queue

            transfer_buffer.stageForTransfer(getBuffer(), _layout.getType(index).getSize(), _offsets.at(index), data);

        }

        ///////////////////////////// protected helper functions /////////////////////////////

        std::vector<uint32_t> UniformBuffer::calcOffsets(const BufferLayout& layout) {
            // calculates the offsets for each of the types in the layout to
            // comply to the alignment rules of uniform buffers

            std::vector<uint32_t> offsets;

            uint32_t offset = 0;
            uint32_t last_size = 0;

            for(int i = 0; i < layout.m_types.size(); i++) {

                const FixedType& type = layout.getType(i);

                // moving past the last type
                offset += last_size;

                // calculating the correct offset for the current type
                uint32_t current_size = layout.getType(i).getSize();
                uint32_t alignment = current_size;

                // alignment rules taken from https://stackoverflow.com/questions/45638520/ubos-and-their-alignments-in-vulkan
                if(type.getNumComp() == 1) 
                    alignment = type.getCompSize(); // A scalar of size N has a base alignment of N.
                else if(type.getNumComp() == 2) 
                    alignment = 2 * type.getCompSize(); // A two-component vector, with components of size N, has a base alignment of 2 N.
                else if(type.getNumComp() == 3 || type.getNumComp() == 4)
                    alignment = 4 * type.getCompSize(); // A three- or four-component vector, with components of size N, has a base alignment of 4 N.
                else // for 3*3 and 4*4 matrices this should work
                    alignment = 16;

                //alignment = std::max(alignment, 4u); // nothing smaller than 4 bytes
                //alignment = std::min(alignment, 16u); // types bigger than 16 bytes still have to be aligned as if they were 16 bytes

                if(offset % alignment)
                    offset += alignment - (offset % alignment);

                offsets.push_back(offset);

                last_size = current_size;
            }

            return offsets;
        }

    } // vulkan

} // undicht