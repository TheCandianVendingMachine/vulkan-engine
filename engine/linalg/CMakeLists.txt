target_sources(engine PRIVATE
    "${ENGINE_HEADER_PATH}/linalg/linalg.h"
    "${ENGINE_HEADER_PATH}/linalg/matrix.h"
    "${ENGINE_HEADER_PATH}/linalg/vector.h"
)
target_sources(engine PRIVATE
    linalg.cpp
    matrix.cpp
    matrix2_operations.cpp
    matrix3_operations.cpp
    matrix4_operations.cpp
    vector.cpp
    vector2_operations.cpp
    vector3_operations.cpp
    vector4_operations.cpp
)
