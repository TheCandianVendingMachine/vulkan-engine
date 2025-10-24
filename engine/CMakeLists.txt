set(ENGINE_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/engine"
)
set(LINALG_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/linalg"
)

set(HEADER_LIST
    "${ENGINE_HEADER_PATH}/meta_defines.h"
    "${ENGINE_HEADER_PATH}/stb_image_write_wrapped.h"
    "${ENGINE_HEADER_PATH}/bitset.h"
    "${ENGINE_HEADER_PATH}/engine.h"
    "${ENGINE_HEADER_PATH}/logger.h"
    "${ENGINE_HEADER_PATH}/random.h"
    "${ENGINE_HEADER_PATH}/newtype.h"
    "${ENGINE_HEADER_PATH}/ecs/default.h"
    "${ENGINE_HEADER_PATH}/ecs/defines.h"
    "${ENGINE_HEADER_PATH}/ecs/entity.h"
    "${ENGINE_HEADER_PATH}/ecs/component.h"
    "${ENGINE_HEADER_PATH}/ecs/system.h"
    "${ENGINE_HEADER_PATH}/ecs/query.h"
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
    stb_image_implementation.cpp
    bitset.cpp
    engine.cpp
    logger.cpp
    random.cpp
    linalg/linalg.cpp
    linalg/vector.cpp
    linalg/matrix.cpp
    ecs/default.cpp
    ecs/entity.cpp
    ecs/component.cpp
    ecs/system.cpp
    ecs/query.cpp
    reflection/type.cpp
    ${HEADER_LIST}
)
target_compile_definitions(engine PRIVATE
    IS_ENGINE
)

target_include_directories(engine PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${EXTERNAL_HEADER_DIR}
    ${EXTERNAL_HEADER_DIR}/stb
    ${EXTERNAL_HEADER_DIR}/fmt
    ${EXTERNAL_HEADER_DIR}/VulkanMemoryAllocator
    ${EXTERNAL_HEADER_DIR}/volk
    ${EXTERNAL_HEADER_DIR}/robin-map
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
    Vulkan::Vulkan
    fastgltf::fastgltf
    fmt::fmt
    volk::volk
    SDL3::SDL3
)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header files"
    FILES ${HEADER_LIST}
)
