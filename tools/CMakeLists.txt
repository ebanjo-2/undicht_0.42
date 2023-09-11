#stb
add_subdirectory(extern/stb)

#assimp
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)
add_subdirectory(extern/assimp)

add_library("tools" 


	src/math/math_tools.h
	src/math/math_tools.cpp
	src/math/orientation_3d.h
	src/math/orientation_3d.cpp
	src/math/orthographic_projection.h
	src/math/orthographic_projection.cpp
	src/math/perspective_projection.h
	src/math/perspective_projection.cpp
	
	src/3D/camera/camera_3d.h
	src/3D/camera/camera_3d.cpp
	src/3D/camera/perspective_camera_3d.h
	src/3D/camera/perspective_camera_3d.cpp
	
	src/xml/xml_tag_attribute.h
	src/xml/xml_tag_attribute.cpp
	src/xml/xml_file.h
	src/xml/xml_file.cpp
	src/xml/xml_element.h
	src/xml/xml_element.cpp
	
	src/binary_data/binary_data_buffer.h
	src/binary_data/binary_data_buffer.cpp
	src/binary_data/binary_data_file.h
	src/binary_data/binary_data_file.cpp
	
	src/scene_loader/scene_loader.h
    src/scene_loader/scene_loader.cpp
	src/scene_loader/texture_loader.h
	src/scene_loader/texture_loader.cpp
	
	extern/stb_implementation.cpp
)


target_include_directories("tools" PUBLIC src extern/glm extern/assimp/include)
target_link_libraries("tools" PUBLIC core graphics stb_image assimp)
