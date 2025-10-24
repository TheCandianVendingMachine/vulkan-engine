set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(LINALG_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/linalg"
)

set(HEADER_LIST
    "${LINALG_HEADER_PATH}/matrix.h"
    "${LINALG_HEADER_PATH}/vector.h"
    "${LINALG_HEADER_PATH}/vector_ops.h"
    "${LINALG_HEADER_PATH}/matrix_ops.h"
)

add_library(linalg_scalar SHARED
    matrix_interface.cpp
    matrix2.cpp
    matrix3.cpp
    matrix4.cpp
    vector_interface.cpp
    vector2.cpp
    vector3.cpp
    vector4.cpp
    ${HEADER_LIST}
)
target_include_directories(linalg_scalar PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_compile_features(linalg_scalar PRIVATE cxx_std_23)

if (MSVC)
    target_compile_definitions(linalg_scalar PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()


add_library(linalg_sse SHARED
    matrix_interface.cpp
    matrix4_sse.cpp
    vector_interface.cpp
    vector2_sse.cpp
    vector3_sse.cpp
    vector4_sse.cpp
    ${HEADER_LIST}
)
target_include_directories(linalg_sse PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_compile_features(linalg_sse PRIVATE cxx_std_23)

if (MSVC)
    target_compile_options(linalg_sse PRIVATE
        /arch:SSE4.2
    )
    target_compile_definitions(linalg_sse PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()


add_library(linalg_avx SHARED
    vector_avx.cpp
    ${HEADER_LIST}
)
target_include_directories(linalg_avx PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_compile_features(linalg_avx PRIVATE cxx_std_23)

if (MSVC)
    target_compile_options(linalg_avx PRIVATE
        /arch:AVX2
    )
    target_compile_definitions(linalg_avx PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header files"
    FILES ${HEADER_LIST}
)
