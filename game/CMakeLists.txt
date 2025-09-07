set(HEADER_LIST
    "${PROJECT_SOURCE_DIR}/include/game/game.hpp")

add_executable(game main.cpp ${HEADER_LIST})
target_compile_features(game PRIVATE cxx_std_23)

target_link_libraries(game PRIVATE engine)
