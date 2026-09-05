set(ENGINE_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/engine"
)
set(LINALG_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/linalg"
)

add_library(engine)
add_subdirectory(assets)
add_subdirectory(ecs)
add_subdirectory(fileio)
add_subdirectory(graphics)
add_subdirectory(linalg)
add_subdirectory(pool)
add_subdirectory(reflection)
add_subdirectory(state)
add_subdirectory(utilities)
target_sources(engine PRIVATE
    "${ENGINE_HEADER_PATH}/bitset.h"
    "${ENGINE_HEADER_PATH}/deletion_queue.h"
    "${ENGINE_HEADER_PATH}/engine.h"
    "${ENGINE_HEADER_PATH}/engine_utils.h"
    "${ENGINE_HEADER_PATH}/logger.h"
    "${ENGINE_HEADER_PATH}/meta_defines.h"
    "${ENGINE_HEADER_PATH}/newtype.h"
    "${ENGINE_HEADER_PATH}/pool.h"
    "${ENGINE_HEADER_PATH}/random.h"
    "${ENGINE_HEADER_PATH}/rwlock.h"
    "${ENGINE_HEADER_PATH}/stb_image_write_wrapped.h"
    "${ENGINE_HEADER_PATH}/version.h")
target_sources(engine PRIVATE
    stb_image_implementation.cpp
    shared_library_linux.cpp
    shared_library_windows.cpp
    deletion_queue.cpp
    deletions.cpp
    engine.cpp
    bitset.cpp
    engine_utils.cpp
    random.cpp
    logger.cpp
    version.cpp)
get_property(ENGINE_SOURCES_ALL TARGET engine PROPERTY SOURCES)
set(ENGINE_SOURCES)
set(ENGINE_HEADERS)
foreach(ENGINE_SOURCE_PATH IN ITEMS ${ENGINE_SOURCES_ALL})
    if(ENGINE_SOURCE_PATH MATCHES ".*\\.cpp")
        set(ENGINE_SOURCES ${ENGINE_SOURCES} ${ENGINE_SOURCE_PATH})
    else()
        set(ENGINE_HEADERS ${ENGINE_HEADERS} ${ENGINE_SOURCE_PATH})
    endif()
endforeach()
source_group(
    TREE "${PROJECT_SOURCE_DIR}/engine"
    PREFIX "Engine"
    FILES
    ${ENGINE_SOURCES}
)
source_group(
    TREE "${PROJECT_SOURCE_DIR}/include/engine"
    PREFIX "Headers"
    FILES
    ${ENGINE_HEADERS}
)
target_sources(engine PRIVATE "${PROJECT_SOURCE_DIR}/include/imconfig.h")
source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Configs"
    FILES
    "${PROJECT_SOURCE_DIR}/include/imconfig.h"
)

target_sources(engine PRIVATE
    ${EXTERNAL_HEADER_DIR}/imgui/imgui.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_demo.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_draw.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_tables.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_widgets.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/backends/imgui_impl_sdl3.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/backends/imgui_impl_vulkan.cpp)
source_group(
    TREE "${EXTERNAL_HEADER_DIR}/Tracy"
    PREFIX "External/Tracy"
    FILES
    ${EXTERNAL_HEADER_DIR}/Tracy/TracyClient.cpp
)
source_group(
    TREE "${EXTERNAL_HEADER_DIR}/imgui"
    PREFIX "External/Imgui"
    FILES
    ${EXTERNAL_HEADER_DIR}/imgui/imgui.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_demo.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_draw.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_tables.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_widgets.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/backends/imgui_impl_sdl3.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/backends/imgui_impl_vulkan.cpp
)

target_compile_definitions(engine PRIVATE
    IS_ENGINE
)

target_include_directories(engine PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(engine SYSTEM PRIVATE
    ${EXTERNAL_HEADER_DIR}
    ${EXTERNAL_HEADER_DIR}/imgui/backends
)
# Headers needed by public API (Vulkan types in engine.h, Tracy in public headers, etc.)
target_include_directories(engine SYSTEM PUBLIC
    ${EXTERNAL_HEADER_DIR}/imgui
    ${EXTERNAL_HEADER_DIR}/volk
    ${EXTERNAL_HEADER_DIR}/VulkanMemoryAllocator
    ${EXTERNAL_HEADER_DIR}/vulkan-headers
    ${EXTERNAL_DIR}/fmtlib/include
    ${EXTERNAL_HEADER_DIR}/Tracy
    ${EXTERNAL_HEADER_DIR}/stb
    ${EXTERNAL_HEADER_DIR}/robin-map
    ${EXTERNAL_DIR}/SDL/include
    ${EXTERNAL_DIR}/fastgltf/include
)

target_compile_features(engine PRIVATE cxx_std_23)

if(MSVC)
    # Flags accepted by both MSVC and clang-cl
    target_compile_options(engine PRIVATE
        /utf-8
        /W4
        /WX
        /wd4201  # nonstandard extension: nameless struct/union
    )
 
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Pure MSVC only — clang-cl suppresses external warnings via SYSTEM
        # includes above and does not support these flags
        target_compile_options(engine PRIVATE
            /external:anglebrackets
            /external:W0
        )
    endif()
 
    target_compile_definitions(engine PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    target_sources(engine INTERFACE
        $<INSTALL_INTERFACE:${NATVIS_DIR}/pool.natvis>
        $<BUILD_INTERFACE:${NATVIS_DIR}/pool.natvis>
        $<INSTALL_INTERFACE:${NATVIS_DIR}/tsl-robin-map.natvis>
        $<BUILD_INTERFACE:${NATVIS_DIR}/tsl-robin-map.natvis>
    )
else()
    # GCC / clang++ (non-Windows or non-MSVC-frontend Clang)
    target_compile_options(engine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wno-gnu-anonymous-struct
        -Wno-nested-anon-types
    )
endif()

target_compile_definitions(engine PRIVATE
    VK_NO_PROTOTYPES
    TRACY_VK_USE_SYMBOL_TABLE
)

target_link_libraries(engine PRIVATE
    fastgltf::fastgltf
    fmt::fmt
    volk
    SDL3::SDL3-static
    PUBLIC
    Tracy::TracyClient
)
