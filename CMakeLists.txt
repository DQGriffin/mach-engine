cmake_minimum_required(VERSION 3.31)
project(mach)

set(CMAKE_CXX_STANDARD 23)

add_library(mach STATIC library.cpp
        engine/fs/Path.cpp
        engine/fs/Path.h
        engine/fs/fs.h
        engine/archive/Archive.cpp
        engine/archive/Archive.h
        engine/logging/logging.h
        ${CMAKE_SOURCE_DIR}/dependencies/glad/src/glad.c
        engine/window/Window.cpp
        engine/window/Window.h
        engine/math/vec.h
        engine/color/color.h
        engine/input/input.cpp
        engine/input/input.h
        engine/input/keys.h
        engine/rendering/texture/texture.cpp
        engine/rendering/texture/texture.h
        engine/assets/AssetManager.cpp
        engine/assets/AssetManager.h
        engine/compression/lz4.cpp
        engine/compression/lz4.h
        engine/compression/compress.h
        engine/time/time.h
        engine/rendering/FpsCounter.h
        engine/rendering/FrameLimiter.h
        engine/rendering/shader/Shader.h
        engine/external/stb_image.h
        engine/rendering/camera/Camera.h
        engine/rendering/primatives/vertex.h
        engine/rendering/primatives/Texture.h
        engine/rendering/primatives/Mesh.cpp
        engine/rendering/primatives/Mesh.h
        engine/rendering/primatives/Model.cpp
        engine/rendering/primatives/Model.h)

add_executable(testbed
        testbed/main.cpp)

target_include_directories(mach PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/glfw/include
        ${CMAKE_SOURCE_DIR}/dependencies/glm/include
        ${CMAKE_SOURCE_DIR}/dependencies/glad/include
        ${CMAKE_SOURCE_DIR}/dependencies/assimp/include
)

target_include_directories(testbed PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/glfw/include
        ${CMAKE_SOURCE_DIR}/dependencies/glm/include
        ${CMAKE_SOURCE_DIR}/dependencies/glad/include
        ${CMAKE_SOURCE_DIR}/dependencies/assimp/include
)

target_link_libraries(testbed PRIVATE mach)
add_definitions(-DENABLE_LOGGING)

find_package(OpenGL REQUIRED)

if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
endif()
if(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREVIDEO_FRAMEWORK CoreVideo)

    target_link_libraries(mach PUBLIC
            ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib/libglfw3.a
            ${CMAKE_SOURCE_DIR}/dependencies/assimp/lib/libassimpd.5.4.3.dylib
            ${OPENGL_gl_LIBRARY}
            ${COCOA_FRAMEWORK}
            ${IOKIT_FRAMEWORK}
            ${COREVIDEO_FRAMEWORK}
)
    add_definitions(-DPLATFORM_APPLE)
endif()