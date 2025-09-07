add_executable(game main.cpp)
target_include_directories(game PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_compile_features(game PRIVATE cxx_std_23)
target_link_libraries(game PRIVATE engine)
