add_library(engine STATIC

    src/engine.h
    src/engine.cpp
    
    src/application.h
    src/application.cpp

)

target_link_libraries(engine core graphics)

target_include_directories(engine PUBLIC src)

