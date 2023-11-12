add_library("physics"
    src/physics.h
    src/physics.cpp
)

target_include_directories("physics" PUBLIC src)

# linking JoltPhysics
add_subdirectory(extern/JoltPhysics/Build)
target_link_libraries("physics" PUBLIC Jolt)