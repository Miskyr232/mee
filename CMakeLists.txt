cmake_minimum_required(VERSION 3.20)

project(mee
    VERSION 0.1.0
    DESCRIPTION "A lightweight ECS-based 2D game engine"
    LANGUAGES CXX
)

option(MEE_BUILD_TESTS "Build mee tests" ON)
option(MEE_BUILD_EXAMPLES "Build mee examples" OFF)

find_package(raylib REQUIRED)

add_library(mee STATIC)

add_library(mee::mee ALIAS mee)

target_sources(mee PRIVATE
    src/ecs/ecs.cpp
    src/core/world.cpp
    src/core/pipeline.cpp

    src/render/particle-manager.cpp
    src/render/texture-manager.cpp
    src/render/renderer.cpp

    src/sound/sound-manager.cpp

    src/world-resources/input-state.cpp
    src/world-resources/camera.cpp

    src/utils/math.cpp
    src/utils/color.cpp
    src/utils/random.cpp

    src/systems/core/lifetime.cpp
    src/systems/render/render-sprite.cpp
    src/systems/render/camera-target.cpp
    src/systems/physics/aabb-collision.cpp
    src/systems/physics/move.cpp
)

target_compile_features(mee PUBLIC cxx_std_20)

target_include_directories(mee PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(mee PUBLIC raylib)

if(MEE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(MEE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
