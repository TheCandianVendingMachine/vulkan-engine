add_executable(game main.cpp)
target_include_directories(game PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_compile_features(game PRIVATE cxx_std_23)
if (MSVC)
    target_compile_options(game PRIVATE
        /utf-8
        /W4
        /WX
        /external:anglebrackets
        /external:W0
    )
    target_compile_definitions(game PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
else()
    target_compile_options(game PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
endif()
target_link_libraries(game PRIVATE engine)
