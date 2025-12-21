set(ENGINE_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/engine"
)
set(LINALG_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/linalg"
)

set(HEADER_LIST
    "${PROJECT_SOURCE_DIR}/include/imconfig.h"
    "${ENGINE_HEADER_PATH}/meta_defines.h"
    "${ENGINE_HEADER_PATH}/stb_image_write_wrapped.h"
    "${ENGINE_HEADER_PATH}/bitset.h"
    "${ENGINE_HEADER_PATH}/deletion_queue.h"
    "${ENGINE_HEADER_PATH}/engine.h"
    "${ENGINE_HEADER_PATH}/engine_utils.h"
    "${ENGINE_HEADER_PATH}/logger.h"
    "${ENGINE_HEADER_PATH}/random.h"
    "${ENGINE_HEADER_PATH}/rwlock.h"
    "${ENGINE_HEADER_PATH}/newtype.h"
    "${ENGINE_HEADER_PATH}/version.h"
    "${ENGINE_HEADER_PATH}/assets/library.h"
    "${ENGINE_HEADER_PATH}/ecs/default.h"
    "${ENGINE_HEADER_PATH}/ecs/defines.h"
    "${ENGINE_HEADER_PATH}/ecs/entity.h"
    "${ENGINE_HEADER_PATH}/ecs/component.h"
    "${ENGINE_HEADER_PATH}/ecs/system.h"
    "${ENGINE_HEADER_PATH}/ecs/query.h"
    "${ENGINE_HEADER_PATH}/fileio/file.h"
    "${ENGINE_HEADER_PATH}/fileio/error.h"
    "${ENGINE_HEADER_PATH}/graphics/descriptor.h"
    "${ENGINE_HEADER_PATH}/graphics/graphics.h"
    "${ENGINE_HEADER_PATH}/graphics/types.h"
    "${ENGINE_HEADER_PATH}/graphics/util.h"
    "${ENGINE_HEADER_PATH}/graphics/pipeline.h"
    "${ENGINE_HEADER_PATH}/graphics/vulkan.h"
    "${ENGINE_HEADER_PATH}/pool.h"
    "${ENGINE_HEADER_PATH}/pool/types.h"
    "${ENGINE_HEADER_PATH}/pool/region.h"
    "${ENGINE_HEADER_PATH}/reflection/type.h"
    "${ENGINE_HEADER_PATH}/reflection/type_float.h"
    "${ENGINE_HEADER_PATH}/reflection/type_int8.h"
    "${ENGINE_HEADER_PATH}/reflection/type_int16.h"
    "${ENGINE_HEADER_PATH}/reflection/type_int32.h"
    "${ENGINE_HEADER_PATH}/reflection/type_int64.h"
    "${ENGINE_HEADER_PATH}/linalg/linalg.h"
    "${ENGINE_HEADER_PATH}/linalg/vector.h"
    "${ENGINE_HEADER_PATH}/linalg/matrix.h"
)

add_library(engine
    ${EXTERNAL_HEADER_DIR}/Tracy/TracyClient.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_demo.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_draw.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_tables.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/imgui_widgets.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/backends/imgui_impl_sdl3.cpp
    ${EXTERNAL_HEADER_DIR}/imgui/backends/imgui_impl_vulkan.cpp
    ${HEADER_LIST}
)
add_subdirectory(assets)
add_subdirectory(ecs)
add_subdirectory(fileio)
add_subdirectory(graphics)
add_subdirectory(linalg)
add_subdirectory(reflection)
target_sources(engine PRIVATE
    stb_image_implementation.cpp
    bitset.cpp
    deletion_queue.cpp
    deletions.cpp
    engine.cpp
    engine_utils.cpp
    logger.cpp
    random.cpp
    version.cpp)
target_compile_definitions(engine PRIVATE
    IS_ENGINE
)

target_include_directories(engine PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${EXTERNAL_HEADER_DIR}
    ${EXTERNAL_HEADER_DIR}/stb
    ${EXTERNAL_HEADER_DIR}/fmt
    ${EXTERNAL_HEADER_DIR}/vulkan-headers
    ${EXTERNAL_HEADER_DIR}/VulkanMemoryAllocator
    ${EXTERNAL_HEADER_DIR}/volk
    ${EXTERNAL_HEADER_DIR}/robin-map
    ${EXTERNAL_HEADER_DIR}/Tracy
    ${EXTERNAL_HEADER_DIR}/imgui
    ${EXTERNAL_HEADER_DIR}/imgui/backends
    ${EXTERNAL_DIR}/fastgltf/include
    ${EXTERNAL_DIR}/fmtlib/include
    ${EXTERNAL_DIR}/sdl/include
)
target_compile_features(engine PRIVATE cxx_std_23)

if (MSVC)
    target_compile_options(engine PRIVATE
        /utf-8
        /W4
        /WX
        /wd4201
        /external:anglebrackets
        /external:W0
    )
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
    target_compile_options(engine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
endif()

target_link_libraries(engine PRIVATE
    fastgltf::fastgltf
    fmt::fmt
    volk
    SDL3::SDL3-static
    PUBLIC
    Tracy::TracyClient
)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header files"
    FILES ${HEADER_LIST}
)
