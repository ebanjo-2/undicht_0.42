add_executable(hello_triangle src/main.cpp)
target_link_libraries(hello_triangle core graphics engine)
set_target_properties(hello_triangle PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# target to build the shaders for the hello_triangle example
add_custom_target(hello_triangle_shader COMMAND python3 ${SHADER_COMPILE_SCRIPT} "${CMAKE_CURRENT_SOURCE_DIR}/src/shader")
add_dependencies(hello_triangle hello_triangle_shader)
