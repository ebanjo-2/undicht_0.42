add_executable(textured_cube src/main.cpp)
target_link_libraries(textured_cube core graphics engine tools)
set_target_properties(textured_cube PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
