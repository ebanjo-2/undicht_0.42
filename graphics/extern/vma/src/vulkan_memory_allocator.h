#ifndef VMA_GLOBAL_ALLOCATOR_H
#define VMA_GLOBAL_ALLOCATOR_H

#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan.h"

namespace undicht {

	namespace vma {
		
		class VulkanMemoryAllocator {
			
		  protected:
		  	
		  	VmaAllocator _allocator;

		  public:

			void init(VkInstance instance, VkDevice device, VkPhysicalDevice physical_device);
			void cleanUp();

			VmaAllocator& getVmaAllocator();
		
		};
	
	} // vma

} // undicht

#endif // VMA_GLOBAL_ALLOCATOR_H
