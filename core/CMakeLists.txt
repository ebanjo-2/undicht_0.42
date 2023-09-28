add_library("core" STATIC

	src/config.h

	src/types.h
	src/types.cpp

	src/buffer_layout.h
	src/buffer_layout.cpp
	
	src/debug.h
	src/debug.cpp
	
	src/file_tools.h
	src/file_tools.cpp
	
	src/profiler.h
	src/profiler.cpp
	        
)

target_include_directories("core" PUBLIC src)
