cmake_minimum_required(VERSION 3.31)
project(mach)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(dependencies/bullet)
add_subdirectory(dependencies/lua)

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
        engine/rendering/primatives/Model.h
        engine/external/entt.h
        engine/ecs/components/Transform.h
        engine/ecs/components/MeshRender.h
        engine/ecs/registry/Registry.cpp
        engine/ecs/registry/Registry.h
        engine/ecs/systems/MeshRendererSystem.h
        engine/math/mat.h
        engine/ecs/Scene.cpp
        engine/ecs/Scene.h
        engine/ecs/components/Name.h
        engine/ecs/Entity.h
        engine/ecs/Entity.cpp
        engine/serialization/SceneSerializer.h
        engine/serialization/BinarySceneSerializer.cpp
        engine/serialization/BinarySceneSerializer.h
        engine/ecs/components/RigidBody.h
        engine/ecs/systems/PhysicsSystem.h
        engine/rendering/debug/DebugLineRenderer.cpp
        engine/rendering/debug/DebugLineRenderer.h
        engine/rendering/debug/BulletDebugDrawer.h
        engine/physics/AABB.h
        engine/ecs/components/Camera.h
        engine/ecs/systems/CameraSystem.h
        engine/physics/PhysicsEngine.cpp
        engine/physics/PhysicsEngine.h
        engine/scripting/lua/types/lua_types.h
        engine/ecs/components/ScriptComponent.h
        engine/ecs/systems/ScriptSystem.h
        engine/scripting/lua/types/StateManager.h
        engine/ecs/components/PendingDeletion.h
        engine/ecs/components/Anchor.h
        engine/ecs/systems/AnchorSystem.h
        engine/ecs/components/FpsCharacterController.h
        engine/ecs/systems/FpsCharacterControllerSystem.h
        engine/ecs/components/MeshCollider.h
        engine/ecs/components/CapsuleCollider.h
        engine/assets/loaders/shaders.h
        engine/serialization/SceneSerialization.h
        engine/serialization/PlainTextSceneSerializer.cpp
        engine/serialization/PlainTextSceneSerializer.h
        engine/utils/string_utils.h
        engine/ecs/components/PointLight.h
        engine/ecs/components/Zone.h
        engine/ecs/systems/ZoneSystem.h
        engine/ecs/components/ZoneTrigger.h
)

target_link_libraries(mach
        PRIVATE
        BulletDynamics
        BulletCollision
        LinearMath
)

target_link_libraries(mach PRIVATE liblua_static)

add_executable(testbed
        testbed/main.cpp)

file(GLOB IMGUI_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imGui/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imGui/*.h"
)

add_executable(studio
        mach-studio/main.cpp
        ${IMGUI_SOURCES}
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imconfig.h
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imgui_demo.cpp
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imgui_draw.cpp
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imgui_internal.h
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imgui_tables.cpp
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imgui_widgets.cpp
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imgui.cpp
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imgui.h
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imstb_rectpack.h
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imstb_textedit.h
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/imstb_truetype.h

        ${CMAKE_SOURCE_DIR}/dependencies/imGui/backends/imgui_impl_opengl3_loader.h
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/backends/imgui_impl_opengl3.cpp
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/backends/imgui_impl_opengl3.h
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/backends/imgui_impl_glfw.cpp

        ${CMAKE_SOURCE_DIR}/dependencies/imGui/misc/cpp/imgui_stdlib.h
        ${CMAKE_SOURCE_DIR}/dependencies/imGui/misc/cpp/imgui_stdlib.cpp
)


target_include_directories(mach PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/glfw/include
        ${CMAKE_SOURCE_DIR}/dependencies/glm/include
        ${CMAKE_SOURCE_DIR}/dependencies/glad/include
        ${CMAKE_SOURCE_DIR}/dependencies/assimp/include
        ${CMAKE_SOURCE_DIR}/dependencies/bullet/src
        ${CMAKE_SOURCE_DIR}/dependencies/sol2/include
        ${CMAKE_SOURCE_DIR}/dependencies/lua/src
        ${CMAKE_SOURCE_DIR}/dependencies/cereal/include
)

target_include_directories(testbed PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/glfw/include
        ${CMAKE_SOURCE_DIR}/dependencies/glm/include
        ${CMAKE_SOURCE_DIR}/dependencies/glad/include
        ${CMAKE_SOURCE_DIR}/dependencies/assimp/include
        ${CMAKE_SOURCE_DIR}/dependencies/bullet/src
        ${CMAKE_SOURCE_DIR}/dependencies/sol2/include
        ${CMAKE_SOURCE_DIR}/dependencies/lua/src
        ${CMAKE_SOURCE_DIR}/dependencies/cereal/include
)

target_include_directories(studio PRIVATE
        ${CMAKE_SOURCE_DIR}/engine
        ${CMAKE_SOURCE_DIR}/dependencies/glfw/include
        ${CMAKE_SOURCE_DIR}/dependencies/glm/include
        ${CMAKE_SOURCE_DIR}/dependencies/glad/include
        ${CMAKE_SOURCE_DIR}/dependencies/assimp/include
        ${CMAKE_SOURCE_DIR}/dependencies/bullet/src
        ${CMAKE_SOURCE_DIR}/dependencies/sol2/include
        ${CMAKE_SOURCE_DIR}/dependencies/lua/src
        ${CMAKE_SOURCE_DIR}/dependencies/cereal/include
        ${CMAKE_SOURCE_DIR}/dependencies/imGui
)

target_link_libraries(testbed PRIVATE mach)
target_link_libraries(studio PRIVATE mach)
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