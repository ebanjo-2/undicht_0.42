#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include "core/vulkan/buffer.h"
#include "core/vulkan/logical_device.h"
#include "buffer_layout.h"


namespace undicht {

    namespace vulkan {

        class UniformBuffer : public Buffer {
            // there are special alignment rules for uniform buffers

        protected:

            BufferLayout _layout;
            std::vector<uint32_t> _offsets;
            uint32_t _buffer_size;

        public:

            void init(const LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, const BufferLayout& layout);
            void cleanUp();

            void setData(uint32_t index, const void* data);

        protected:
            // protected helper functions

            // calculates the offsets for each of the types in the layout to 
            // comply to the alignment rules of uniform buffers
            std::vector<uint32_t> static calcOffsets(const BufferLayout& layout);

        };

    } // vulkan

} // undicht

#endif // UNIFORM_BUFFER_H