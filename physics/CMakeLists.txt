add_library("physics"

	src/math/physics_types.h
	
	src/primitives/sphere.h
	src/primitives/sphere.cpp

	src/primitives/cuboid.h
	src/primitives/cuboid.cpp

	src/primitives/object.h
	src/primitives/object.cpp
	
	src/objects/sphere_object.h
	src/objects/sphere_object.cpp

	src/objects/cuboid_object.h
	src/objects/cuboid_object.cpp

	src/simulation/physics_simulation.h
	src/simulation/physics_simulation.cpp

	src/simulation/collision.h
	src/simulation/collision.cpp

	src/simulation/collision_detection.h
	src/simulation/collision_detection.cpp

)

target_include_directories("physics" PUBLIC src)
target_link_libraries("physics" PUBLIC core tools)
