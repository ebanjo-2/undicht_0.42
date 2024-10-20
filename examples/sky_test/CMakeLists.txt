add_executable(sky_test 
    src/main.cpp
    src/sky_renderer.h
    src/sky_renderer.cpp
)


target_link_libraries(sky_test core graphics engine tools)
set_target_properties(sky_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# target to build the shaders for the sky_test_shader example
add_custom_target(sky_test_shader COMMAND python3 ${SHADER_COMPILE_SCRIPT} "${CMAKE_CURRENT_SOURCE_DIR}/src/shader")
add_dependencies(sky_test sky_test_shader)