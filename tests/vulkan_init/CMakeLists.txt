add_executable(vulkan_init_test src/main.cpp)
target_link_libraries(vulkan_init_test core graphics)
add_test(NAME vulkan_init_test COMMAND vulkan_init_test)
