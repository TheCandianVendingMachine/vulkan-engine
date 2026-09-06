target_sources(engine PRIVATE
    "${ENGINE_HEADER_PATH}/reflection/type.h"
    "${ENGINE_HEADER_PATH}/reflection/type_float.h"
    "${ENGINE_HEADER_PATH}/reflection/type_int8.h"
    "${ENGINE_HEADER_PATH}/reflection/type_int16.h"
    "${ENGINE_HEADER_PATH}/reflection/type_int32.h"
    "${ENGINE_HEADER_PATH}/reflection/type_int64.h"
    "${ENGINE_HEADER_PATH}/reflection/type_string.h"
    "${ENGINE_HEADER_PATH}/reflection/type_vector.h"
    "${ENGINE_HEADER_PATH}/reflection/type_quaternion.h"
)
target_sources(engine PRIVATE
    type.cpp
)
