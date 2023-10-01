#ifndef TRANSFER_BUFFER_H
#define TRANSFER_BUFFER_H

#include "core/vulkan/buffer.h"
#include "core/vulkan/image.h"
#include "core/vulkan/command_buffer.h"

#include "vector"

namespace undicht {

    namespace vulkan {

        class TransferBuffer : public Buffer{
          
          protected:

            // structs needed to copy data to a buffer
            struct BufferCopyData {
                VkBuffer _transfer_dst;
                VkBufferCopy _buffer_copy;
            };

            // structs needed to copy data to a buffer
            struct ImageCopyData {
                VkImage _transfer_dst;
                VkBufferImageCopy _image_copy;
                VkImageSubresourceRange _image_subresource_range;
                VkImageMemoryBarrier _first_image_memory_barrier;   
                VkImageMemoryBarrier _second_image_memory_barrier;
            };

          protected:

            std::vector<BufferCopyData> _buffer_copies;
            std::vector<ImageCopyData> _image_copies;

            uint32_t _bytes_stored = 0;

          public:

            void init(vma::VulkanMemoryAllocator& allocator, const std::vector<uint32_t>& queue_ids, uint32_t byte_size);
            void cleanUp();

            /// stores the data in the transfer buffer and creates the info structs necessary to
            /// tell vulkan to copy the data into the destination buffer / image
            void stageForTransfer(VkBuffer dst, uint32_t byte_size, uint32_t offset, const uint8_t* data);
            void stageForTransfer(VkImage dst, const uint8_t* data,  uint32_t byte_size, VkExtent3D data_extent, VkOffset3D offset = {0,0,0}, uint32_t layer = 0, uint32_t mip_level = 0, VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout final_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VkAccessFlags initial_access = VK_ACCESS_NONE, VkAccessFlags final_access = VK_ACCESS_SHADER_READ_BIT);

            /// records the commands needed to copy the data to the buffers / images
            /// if the destination is an image, pipeline barriers will be added to get the image in the correct layout
            /// endCommandBuffer() needs to be called on the command buffer before it can be submitted on a queue 
            void completeTransfers(CommandBuffer& cmd);

            /// @brief call before reusing the transfer buffer
            /// all the transfers should be completed at this point, as the info structs for transfering the data to buffers / images will be deleted
            void reset();

        };

    } // vulkan

} // undicht

#endif // TRANSFER_BUFFER_H