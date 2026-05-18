cmake_minimum_required(VERSION 3.21...4.0)

project(
    VulkanGameThing
    VERSION 1.0
    DESCRIPTION "My foray into Vulkan with a simple game"
    LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# Output directories
# For multi-config generators (Ninja Multi-Config, VS), CMake automatically
# appends /<CONFIG> to these, giving bin/Debug/, bin/Release/, etc.
# ---------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")  # .exe and .dll
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")  # .so (non-Windows)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")  # .lib / .a

set(EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)
set(EXTERNAL_HEADER_DIR ${EXTERNAL_DIR}/include)
set(OPTIONAL_DIR ${PROJECT_SOURCE_DIR}/optional)
set(NATVIS_DIR ${OPTIONAL_DIR}/natvis)

option(BUILD_TESTS "Build engine and game tests" ON)

add_subdirectory(external)

# ---------------------------------------------------------------------------
# Third-party warning suppressions for clang-cl
#
# Some external CMakeLists.txt files use if(MSVC) to add /external: flags,
# not knowing that clang-cl also satisfies MSVC but doesn't support them.
# Rather than patching third-party files, we suppress the resulting
# -Wunused-command-line-argument noise here, after the targets are defined.
# ---------------------------------------------------------------------------
if(MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(fastgltf PRIVATE -Wno-unused-command-line-argument)
endif()

find_package(Vulkan REQUIRED COMPONENTS volk)

add_subdirectory(tools)
add_subdirectory(linalg)
add_subdirectory(engine)
add_subdirectory(game)

# ---------------------------------------------------------------------------
# Assets symlink (with copy fallback)
#
# Delegates to cmake/symlink_or_copy.cmake which:
#   1. Tries to create a symlink: bin/<CONFIG>/assets -> <repo>/assets
#   2. Falls back to copy_directory_if_different if symlink creation fails
#      (e.g. Windows without Developer Mode or admin privileges) and emits
#      a warning explaining how to enable symlinks.
#
# $<CONFIG> resolves to the active config (Debug/Release/RelWithDebInfo)
# at build time, so the correct output folder is always targeted.
# ---------------------------------------------------------------------------
add_custom_target(symlink_assets ALL
    COMMAND ${CMAKE_COMMAND}
        -DSOURCE_PATH=${PROJECT_SOURCE_DIR}/assets
        -DLINK_PATH=${CMAKE_BINARY_DIR}/bin/$<CONFIG>/assets
        -P ${PROJECT_SOURCE_DIR}/cmake/symlink_or_copy.cmake
    COMMENT "Linking/copying assets into bin/$<CONFIG>/"
    VERBATIM
)

# Ensure assets are in place before game.exe runs any post-build steps.
add_dependencies(game symlink_assets)

if(BUILD_TESTS)
    set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
    set(CATCH_INSTALL_EXTRAS ON CACHE BOOL "" FORCE)
    add_subdirectory(external/Catch2)
    add_subdirectory(tests)
endif()