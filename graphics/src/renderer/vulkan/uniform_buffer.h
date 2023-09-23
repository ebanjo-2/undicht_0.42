#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include "core/vulkan/buffer.h"
#include "core/vulkan/logical_device.h"
#include "buffer_layout.h"
#include "transfer_buffer.h"


namespace undicht {

    namespace vulkan {

        class UniformBuffer : public Buffer {
            // there are special alignment rules for uniform buffers

        protected:

            LogicalDevice _device_handle;
            vma::VulkanMemoryAllocator _allocator_handle;

            BufferLayout _layout;
            std::vector<uint32_t> _offsets;
            uint32_t _buffer_size;

        public:

            void init(const LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, const BufferLayout& layout);
            void cleanUp();

            /// @brief will try to directly copy the data to the uniform buffer
            /// this will only be possible, if the memory is cpu visible, which it may not be depending on the graphics card that is used
            /// @return will return false if its not possible to directly copy the data, in which case you need to use uploadData()
            bool setData(uint32_t index, const uint8_t* data);

            /// @brief stores the data in the transfer buffer (needs to have enough memory allocated)
            /// for the data to actually be copied to the uniform buffer,
            /// you need to call completeTransfers() on the transfer buffer and also submit the command buffer to a queue
            void uploadData(uint32_t index, const uint8_t* data, TransferBuffer& transfer_buffer);

        protected:
            // protected helper functions

            // calculates the offsets for each of the types in the layout to 
            // comply to the alignment rules of uniform buffers
            std::vector<uint32_t> static calcOffsets(const BufferLayout& layout);


        };

    } // vulkan

} // undicht

#endif // UNIFORM_BUFFER_H