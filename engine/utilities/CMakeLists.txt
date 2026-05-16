target_sources(engine PRIVATE
    "${ENGINE_HEADER_PATH}/utilities/transform.h"
    "${ENGINE_HEADER_PATH}/utilities/quaternion.h"
)
target_sources(engine PRIVATE
    transform.cpp
    quaternion.cpp
)
