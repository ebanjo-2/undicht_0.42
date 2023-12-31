#ifndef VK_DEBUG_H
#define VK_DEBUG_H

#include "debug.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

// will not abort if there is a vulkan error, but instead will log it
#define VK_CHECK(x)                                                 	  	\
		{																  	\
			VkResult err = x;                                             	\
			if (err) {                                                    	\
				UND_WARNING << "Vulkan: " << string_VkResult(err) << "\n";	\
			}															  	\
		}                                                     
	

// copied from: https://vkguide.dev/docs/chapter-1/vulkan_init_code/
// we want to immediately abort when there is an error. 
// In normal engines this would give an error message to the user, or perform a dump of state.
#define VK_ASSERT(x)                                                		\
		{																	\
			VkResult err = x;                                          		\
			if (err) {                                                  	\
				UND_ERROR << "Vulkan: " << string_VkResult(err) << "\n";	\
				abort();                                                	\
			}																\
		}                                                           

#endif // VK_DEBUG_H