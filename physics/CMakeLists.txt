add_library("physics"

    src/physics.h
    src/physics.cpp

    src/object_layers.h
    src/object_layers.cpp

    src/debug_contact_listener.h
    src/debug_contact_listener.cpp

    src/debug_body_activation_listener.h
    src/debug_body_activation_listener.cpp
)

target_include_directories("physics" PUBLIC src)

# linking JoltPhysics
add_subdirectory(extern/JoltPhysics/Build)
target_link_libraries("physics" PUBLIC Jolt core)