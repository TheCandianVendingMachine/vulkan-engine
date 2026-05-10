target_sources(engine PRIVATE
    "${ENGINE_HEADER_PATH}/state/state.h"
    "${ENGINE_HEADER_PATH}/state/manager.h"
)
target_sources(engine PRIVATE
    manager.cpp
    state.cpp
)
