add_library(engine STATIC

    src/engine.h
    src/engine.cpp
    
    src/application.h
    src/application.cpp
    
    src/frame_manager.h
    src/frame_manager.cpp
    
)

target_link_libraries(engine core graphics)

target_include_directories(engine PUBLIC src)

