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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)
set(EXTERNAL_HEADER_DIR ${EXTERNAL_DIR}/include)
set(OPTIONAL_DIR ${PROJECT_SOURCE_DIR}/optional)
set(NATVIS_DIR ${OPTIONAL_DIR}/natvis)

option(BUILD_TESTS "Build engine and game tests" ON)

add_subdirectory(external)

find_package(Vulkan REQUIRED COMPONENTS volk)

add_subdirectory(tools)
add_subdirectory(linalg)
add_subdirectory(engine)
add_subdirectory(game)

if(BUILD_TESTS)
    set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
    set(CATCH_INSTALL_EXTRAS ON CACHE BOOL "" FORCE)
    add_subdirectory(external/Catch2)
    add_subdirectory(tests)
endif()
