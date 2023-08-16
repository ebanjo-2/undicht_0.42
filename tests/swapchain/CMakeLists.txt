add_executable(swapchain_test src/main.cpp)
target_link_libraries(swapchain_test core graphics engine)
add_test(NAME swapchain_test COMMAND swapchain_test)
