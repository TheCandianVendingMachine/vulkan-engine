target_sources(game PRIVATE
    "${GAME_HEADER_PATH}/tilemap/tilemap.h"
)
target_sources(game PRIVATE
    tilemap_predraw_pipeline.cpp
)
