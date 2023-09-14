#include "transfer_buffer.h"
#include "debug.h"

namespace undicht {

    namespace vulkan {

        void TransferBuffer::init(vma::VulkanMemoryAllocator& allocator, const std::vector<uint32_t>& queue_ids, uint32_t byte_size) {

            VkBufferUsageFlags buffer_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO;
            VmaAllocationCreateFlags allocation_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

            Buffer::init(allocator, queue_ids, byte_size, buffer_usage, memory_usage, allocation_flags);
        }

        void TransferBuffer::cleanUp() {

            Buffer::cleanUp();
        }

        void TransferBuffer::stageForTransfer(VkBuffer dst, uint32_t byte_size, uint32_t offset, const void* data) {
            /// stores the data in the transfer buffer and creates the info structs necessary to
            /// tell vulkan to copy the data into the destination buffer / image

            if(getAllocatedSize() < (_bytes_stored + byte_size)) {
                UND_ERROR << "Failed to store Data in transfer buffer: not enough memory allocated\n";
                return;
            }

            // store the data in the transfer buffer
            Buffer::setData(byte_size, _bytes_stored, data);

            // create the necessary structs for the transfer
            BufferCopyData buffer_copy;
            buffer_copy._transfer_dst = dst;
            buffer_copy._buffer_copy = Buffer::createBufferCopy(byte_size, _bytes_stored, offset);

            _buffer_copies.push_back(buffer_copy);

            _bytes_stored += byte_size;

        }

        void TransferBuffer::stageForTransfer(VkImage dst, const void* data,  uint32_t byte_size, VkExtent3D data_extent, VkOffset3D offset, uint32_t layer, uint32_t mip_level, VkImageLayout initial_layout, VkImageLayout final_layout, VkAccessFlags initial_access, VkAccessFlags final_access) {
            
            if(getAllocatedSize() < (_bytes_stored + byte_size)) {
                UND_ERROR << "Failed to store Data in transfer buffer: not enough memory allocated\n";
                return;
            }

            // store the data in the transfer buffer
            Buffer::setData(byte_size, _bytes_stored, data);

            // create the necessary structs for the transfer
            ImageCopyData image_copy;
            image_copy._transfer_dst = dst;
            image_copy._image_copy = Image::createBufferImageCopy(data_extent, offset, VK_IMAGE_ASPECT_COLOR_BIT, layer, mip_level, _bytes_stored);
            image_copy._image_subresource_range = Image::createImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, mip_level, 1, layer, 1);
            image_copy._first_image_memory_barrier = Image::createImageMemoryBarrier(dst, image_copy._image_subresource_range, initial_layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, initial_access, VK_ACCESS_TRANSFER_WRITE_BIT);   
            image_copy._second_image_memory_barrier = Image::createImageMemoryBarrier(dst, image_copy._image_subresource_range, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, final_layout, VK_ACCESS_TRANSFER_WRITE_BIT, final_access);
            
            _image_copies.push_back(image_copy);

            _bytes_stored += byte_size;

        }

        void TransferBuffer::completeTransfers(CommandBuffer& cmd) {
            /// records the commands needed to copy the data to the buffers / images
            /// if the destination is an image, pipeline barriers will be added to get the image in the correct layout
            /// endCommandBuffer() needs to be called on the command buffer before it can be submitted on a queue 

            for(BufferCopyData& buffer_copy : _buffer_copies) {

                cmd.copy(Buffer::getBuffer(), buffer_copy._transfer_dst, buffer_copy._buffer_copy);
            }

            for(ImageCopyData& image_copy : _image_copies) {
                
                if((image_copy._first_image_memory_barrier.oldLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) || (image_copy._first_image_memory_barrier.srcAccessMask != VK_ACCESS_TRANSFER_WRITE_BIT))
                    cmd.pipelineBarrier(image_copy._first_image_memory_barrier, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT); // transition to layout that allows transfering data to the texture
                
                cmd.copy(Buffer::getBuffer(), image_copy._transfer_dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image_copy._image_copy); // copy data to image
                
                if((image_copy._second_image_memory_barrier.newLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) || (image_copy._second_image_memory_barrier.dstAccessMask != VK_ACCESS_TRANSFER_WRITE_BIT))
                    cmd.pipelineBarrier(image_copy._second_image_memory_barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT); // (transition to layout that allows sampling the texture in a shader)
                
            }

        }

    } // vulkan

} // undicht