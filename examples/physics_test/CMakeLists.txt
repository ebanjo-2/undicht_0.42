add_executable(physics_test 

    src/main.cpp 
    src/physics_renderer.h 
    src/physics_renderer.cpp    
    src/physics_scene.h 
    src/physics_scene.cpp
)

target_link_libraries(physics_test core graphics engine physics)
set_target_properties(physics_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# target to build the shaders for the hello_triangle example
add_custom_target(physics_test_shader COMMAND python3 ${SHADER_COMPILE_SCRIPT} "${CMAKE_CURRENT_SOURCE_DIR}/src/shader")
add_dependencies(physics_test physics_test_shader)
