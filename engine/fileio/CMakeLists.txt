target_sources(engine PRIVATE
    "${ENGINE_HEADER_PATH}/fileio/error.h"
    "${ENGINE_HEADER_PATH}/fileio/file.h"
)
target_sources(engine PRIVATE
    error.cpp
    file.cpp
)
