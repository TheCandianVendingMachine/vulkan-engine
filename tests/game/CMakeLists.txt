add_executable(test_game_input
    test_input_mapping.cpp
    ${PROJECT_SOURCE_DIR}/game/input/mapping.cpp
    ${PROJECT_SOURCE_DIR}/game/input/serializer.cpp
    ${PROJECT_SOURCE_DIR}/game/input/types.cpp
)

target_include_directories(test_game_input PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(test_game_input PRIVATE
    Catch2::Catch2WithMain
    engine
    SDL3::SDL3-static
)

target_compile_features(test_game_input PRIVATE cxx_std_23)

set_target_properties(test_game_input PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)

if(MSVC)
    target_compile_options(test_game_input PRIVATE /utf-8)
    target_compile_definitions(test_game_input PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
endif()

catch_discover_tests(test_game_input
    DL_PATHS "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
