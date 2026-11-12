cmake_minimum_required(VERSION 3.20)

project(mee
    VERSION 0.1.0
    DESCRIPTION "A lightweight ECS-based 2D game engine"
    LANGUAGES CXX
)

option(MEE_BUILD_TESTS "Build mee tests" ON)
option(MEE_BUILD_EXAMPLES "Build mee examples" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_library(mee STATIC)

add_library(mee::mee ALIAS mee)

target_sources(mee PRIVATE
    src/ecs/ecs.cpp
    src/core/engine.cpp
    src/core/pipeline.cpp

    src/render/particle-manager.cpp
    src/render/texture-manager.cpp
    src/render/renderer.cpp

    src/sound/sound-manager.cpp

    src/engine-resources/input-state.cpp
    src/engine-resources/camera.cpp

    src/utils/math.cpp
    src/utils/color.cpp
    src/utils/random.cpp
    src/utils/timer.cpp
    src/utils/text.cpp

    src/systems/core/lifetime.cpp
    src/systems/render/render-sprite.cpp
    src/systems/render/camera-target.cpp
    src/systems/physics/aabb-collision.cpp
    src/systems/physics/move.cpp
)

target_compile_features(mee PUBLIC cxx_std_20)

target_compile_options(mee PRIVATE
    $<$<CONFIG:Release>:-O2>
)

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


include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install library
install(
    TARGETS mee
    EXPORT meeTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install headers
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(
    EXPORT meeTargets
    FILE meeTargets.cmake
    NAMESPACE mee::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mee
)

# Generate package config
configure_package_config_file(
    cmake/meeConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/meeConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mee
)

# Generate version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/meeConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install config files
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/meeConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/meeConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mee
)

