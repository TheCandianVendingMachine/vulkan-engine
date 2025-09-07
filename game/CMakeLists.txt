add_executable(game main.cpp)
target_include_directories(game PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_compile_features(game PRIVATE cxx_std_23)
if (MSVC)
    target_compile_options(game PRIVATE
        /utf-8
    )
    target_compile_definitions(game PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()
target_link_libraries(game PRIVATE engine)
