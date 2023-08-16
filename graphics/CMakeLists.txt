find_package(Vulkan REQUIRED)

set(GRAPHICS_CORE_SOURCES

    src/core/vulkan/vk_debug.h    
    
    src/core/vulkan/instance.h
    src/core/vulkan/instance.cpp
    
    src/core/vulkan/logical_device.h
    src/core/vulkan/logical_device.cpp
    
    src/core/vulkan/swap_chain.h
    src/core/vulkan/swap_chain.cpp
    
    src/core/vulkan/image_view.h
    src/core/vulkan/image_view.cpp

    src/core/vulkan/formats.h
    src/core/vulkan/formats.cpp

)

set(GRAPHICS_WINDOW_SOURCES
    src/window/glfw/window_api.h
    src/window/glfw/window_api.cpp

    src/window/glfw/monitor.h
    src/window/glfw/monitor.cpp

    src/window/glfw/window.h
    src/window/glfw/window.cpp
)

set(GRAPHICS_RENDERER_SOURCES

)

set(GRAPHICS_USER_INTERFACE_SOURCES

)

set(GRAPHICS_IMGUI_SOURCES

)

add_library(graphics
    ${GRAPHICS_WINDOW_SOURCES}
    ${GRAPHICS_CORE_SOURCES}
    ${GRAPHICS_RENDERER_SOURCES}
    ${GRAPHICS_USER_INTERFACE_SOURCES}
    ${GRAPHICS_IMGUI_SOURCES}
)

target_include_directories(graphics PUBLIC src)

# setting the shader compile script path
set(SHADER_COMPILE_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/src/shader/compile.py CACHE STRING "SHADER_COMPILE_SCRIPT") 


# linking undicht core
target_link_libraries(graphics core)

# linking glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw)
target_link_libraries(graphics glfw)

#linking imgui
set(IMGUI_PATH  "extern/imgui")
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 
file(GLOB IMGUI_BACKEND ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp ${IMGUI_PATH}/backends/imgui_impl_vulkan.cpp) 
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_BACKEND})
target_include_directories(imgui PUBLIC extern/imgui ${Vulkan_INCLUDE_DIRS})
target_link_libraries(imgui ${Vulkan_LIBRARIES} glfw)

target_link_libraries(graphics imgui)

# linking vulkan
target_include_directories(graphics PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(graphics ${Vulkan_LIBRARIES})

# linking vma
add_subdirectory(extern/vma)
target_link_libraries(graphics vma)


# adding a custom target for building the engines shaders
add_custom_target(shader COMMAND python3 ${SHADER_COMPILE_SCRIPT} "${PROJECT_SOURCE_DIR}/graphics/src/shader")
add_dependencies(graphics shader)
