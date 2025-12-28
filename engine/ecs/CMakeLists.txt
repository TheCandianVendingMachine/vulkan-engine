target_sources(engine PRIVATE
    "${ENGINE_HEADER_PATH}/ecs/component.h"
    "${ENGINE_HEADER_PATH}/ecs/default.h"
    "${ENGINE_HEADER_PATH}/ecs/defines.h"
    "${ENGINE_HEADER_PATH}/ecs/entity.h"
    "${ENGINE_HEADER_PATH}/ecs/query.h"
    "${ENGINE_HEADER_PATH}/ecs/system.h"
)
target_sources(engine PRIVATE
    component.cpp
    default.cpp
    entity.cpp
    query.cpp
    system.cpp
)
