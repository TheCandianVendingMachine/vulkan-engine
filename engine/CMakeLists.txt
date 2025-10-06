set(ENGINE_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/engine"
)

set(HEADER_LIST
    "${ENGINE_HEADER_PATH}/bitset.h"
    "${ENGINE_HEADER_PATH}/engine.h"
    "${ENGINE_HEADER_PATH}/logger.h"
    "${ENGINE_HEADER_PATH}/random.h"
    "${ENGINE_HEADER_PATH}/ecs/entity.h"
    "${ENGINE_HEADER_PATH}/ecs/component.h"
    "${ENGINE_HEADER_PATH}/ecs/system.h"
    "${ENGINE_HEADER_PATH}/ecs/query.h"
    "${ENGINE_HEADER_PATH}/pool.h"
    "${ENGINE_HEADER_PATH}/pool/types.h"
    "${ENGINE_HEADER_PATH}/pool/region.h"
)

add_library(engine
    bitset.cpp
    engine.cpp
    logger.cpp
    random.cpp
    ecs/entity.cpp
    ecs/component.cpp
    ecs/system.cpp
    ${EXTERNAL_HEADER_DIR}/stb/stb_image_implementation.cpp
    ${HEADER_LIST}
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
    )
    target_compile_definitions(engine PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    target_sources(engine INTERFACE
        $<INSTALL_INTERFACE:${NATVIS_DIR}/pool.natvis>
        $<BUILD_INTERFACE:${NATVIS_DIR}/pool.natvis>
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
