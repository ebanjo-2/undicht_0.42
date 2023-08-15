add_executable(core_test src/main.cpp)
target_link_libraries(core_test core)
add_test(NAME core_test COMMAND core_test)
