add_library("engine" STATIC

    src/engine.h
    src/engine.cpp
    
    src/application.h
    src/application.cpp

    src/scene/scene.h
    src/scene/scene.cpp
    src/scene/node.h
    src/scene/node.cpp
    src/scene/texture.h
    src/scene/texture.cpp
    src/scene/mesh.h
    src/scene/mesh.cpp
    src/scene/material.h
    src/scene/material.cpp
    src/scene/scene_loader.h
    src/scene/scene_loader.cpp

)

target_link_libraries("engine" core graphics tools)

target_include_directories("engine" PUBLIC src)
