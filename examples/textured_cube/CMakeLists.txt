add_executable(textured_cube src/main.cpp)
target_link_libraries(textured_cube core graphics engine tools)
set_target_properties(textured_cube PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# target to build the shaders for the textured_cube example
add_custom_target(textured_cube_shader COMMAND python3 ${SHADER_COMPILE_SCRIPT} "${CMAKE_CURRENT_SOURCE_DIR}/src/shader")
add_dependencies(textured_cube textured_cube_shader)
