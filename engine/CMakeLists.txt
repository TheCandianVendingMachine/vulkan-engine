set(ENGINE_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/engine"
)

set(HEADER_LIST
    "${ENGINE_HEADER_PATH}/engine.h"
)

add_library(engine
    engine.cpp
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
)
target_compile_features(engine PRIVATE cxx_std_23)

target_link_libraries(engine PRIVATE
    Vulkan::Vulkan
    ${FASTGLTF_LIBRARY_RELEASE}
    ${FMTLIB_LIBRARY_RELEASE}
    ${VOLK_LIBRARY_RELEASE}
    ${SDL_LIBRARY_RELEASE}
)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header files"
    FILES ${HEADER_LIST}
)
