add_library("core" STATIC

	src/config.h

	src/types.h
	src/types.cpp

	src/buffer_layout.h
	src/buffer_layout.cpp
	
	src/debug.h
	src/debug.cpp

)

target_include_directories("core" PUBLIC src)
