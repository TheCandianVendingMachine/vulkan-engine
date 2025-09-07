cmake_minimum_required(VERSION 3.15...4.0)

project(
    VulkanGameThing
    VERSION 1.0
    DESCRIPTION "My foray into Vulkan with a simple game"
    LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

set(EXTERNAL_HEADER_DIR ${PROJECT_SOURCE_DIR}/external/include)

find_package(Vulkan REQUIRED COMPONENTS volk)

find_library(FASTGLTF_LIBRARY_DEBUG
    NAMES fastgltf
    HINTS ${PROJECT_SOURCE_DIR}/external/lib/fastgltf/Debug
)
find_library(FASTGLTF_LIBRARY_RELEASE
    NAMES fastgltf
    HINTS ${PROJECT_SOURCE_DIR}/external/lib/fastgltf/Release
)

find_library(FMTLIB_LIBRARY_DEBUG
    NAMES fmtd
    HINTS ${PROJECT_SOURCE_DIR}/external/lib/fmtlib/Debug
)
find_library(FMTLIB_LIBRARY_RELEASE
    NAMES fmt
    HINTS ${PROJECT_SOURCE_DIR}/external/lib/fmtlib/Release
)

find_library(VOLK_LIBRARY_DEBUG
    NAMES volk
    HINTS ${PROJECT_SOURCE_DIR}/external/lib/volk/Debug
)
find_library(VOLK_LIBRARY_RELEASE
    NAMES volk
    HINTS ${PROJECT_SOURCE_DIR}/external/lib/volk/Release
)

find_library(SDL_LIBRARY_DEBUG
    NAMES SDL3-static
    HINTS ${PROJECT_SOURCE_DIR}/external/lib/SDL/Debug/
)
find_library(SDL_LIBRARY_RELEASE
    NAMES SDL3-static
    HINTS ${PROJECT_SOURCE_DIR}/external/lib/SDL/Release
)

add_subdirectory(engine)
add_subdirectory(game)
