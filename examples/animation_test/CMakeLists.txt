add_executable(animation_test 

    src/main.cpp 

)

target_link_libraries(animation_test core graphics engine physics tools)
set_target_properties(animation_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# target to build the shaders for the hello_triangle example
add_custom_target(animation_test_shader COMMAND python3 ${SHADER_COMPILE_SCRIPT} "${CMAKE_CURRENT_SOURCE_DIR}/src/shader")
add_dependencies(animation_test animation_test_shader)
