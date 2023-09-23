#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include "vulkan_memory_allocator.h"

namespace undicht {

    namespace vma {

        void VulkanMemoryAllocator::init(VkInstance instance, VkDevice device, VkPhysicalDevice physical_device) {
            
            VmaAllocatorCreateInfo info{};
            info.device = device;
            info.flags = {};
            info.instance = instance;
            info.pAllocationCallbacks = {}; // optional
            info.pDeviceMemoryCallbacks = {}; // optional
            info.pHeapSizeLimit = NULL; // as far as i understand, its fine to leave as is
            info.physicalDevice = physical_device;
            info.preferredLargeHeapBlockSize = 0; // set to 0 for default, should be fine
            info.pTypeExternalMemoryHandleTypes = NULL; // i have no idea what this does
            info.pVulkanFunctions = NULL; // documentation says can be null, so it is
            info.vulkanApiVersion = VK_API_VERSION_1_3; // optional

            vmaCreateAllocator(&info, &_allocator);
        }

		void VulkanMemoryAllocator::cleanUp() {

            vmaDestroyAllocator(_allocator);
        }

        VmaAllocator& VulkanMemoryAllocator::getVmaAllocator() {

            return _allocator;
        }

    } // vma

} // undicht
