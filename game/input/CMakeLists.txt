target_sources(game PRIVATE
    "${GAME_HEADER_PATH}/input/layer.h"
    "${GAME_HEADER_PATH}/input/mapping.h"
    "${GAME_HEADER_PATH}/input/register.h"
    "${GAME_HEADER_PATH}/input/types.h"
)

target_sources(game PRIVATE
    layer.cpp
    mapping.cpp
    register.cpp
    types.cpp
)
