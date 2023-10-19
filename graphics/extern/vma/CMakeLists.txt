find_package(Vulkan REQUIRED)

add_library(vma
	# the only purpose of this library is the implementation of vma
	src/vulkan_memory_allocator.h
	src/vulkan_memory_allocator.cpp
)

target_include_directories(vma PUBLIC src)

# adding the vma directory (included in the vulkan sdk)
target_include_directories(vma PRIVATE ${Vulkan_INCLUDE_DIRS})
set_property(TARGET vma PROPERTY CXX_STANDARD 17)