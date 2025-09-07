set(ENGINE_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/engine"
)

set(HEADER_LIST
    "${ENGINE_HEADER_PATH}/engine.h"
    "${ENGINE_HEADER_PATH}/logger.h"
)

add_library(engine
    engine.cpp
    logger.cpp
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
endif()

target_link_libraries(engine PRIVATE
    Vulkan::Vulkan
    optimized ${FASTGLTF_LIBRARY_RELEASE} debug ${FASTGLTF_LIBRARY_DEBUG}
    optimized ${FMTLIB_LIBRARY_RELEASE} debug ${FMTLIB_LIBRARY_DEBUG}
    optimized ${VOLK_LIBRARY_RELEASE} debug ${VOLK_LIBRARY_DEBUG}
    optimized ${SDL_LIBRARY_RELEASE} debug ${SDL_LIBRARY_DEBUG}
)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header files"
    FILES ${HEADER_LIST}
)
