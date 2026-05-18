set(GAME_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/game"
)
set(HEADER_LIST
    "${GAME_HEADER_PATH}/game.h"
    "${GAME_HEADER_PATH}/world.h"
)

add_executable(game
    main.cpp
    world.cpp
    ${HEADER_LIST}

)

add_subdirectory(level)
add_subdirectory(tilemap)

target_include_directories(game PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(game SYSTEM PRIVATE
)
 
target_compile_features(game PRIVATE cxx_std_23)
 
if(MSVC)
    # Flags accepted by both MSVC and clang-cl
    target_compile_options(game PRIVATE
        /utf-8
        /W4
        /WX
    )
 
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Pure MSVC only — clang-cl suppresses external warnings via SYSTEM
        # includes above and does not support these flags
        target_compile_options(game PRIVATE
            /external:anglebrackets
            /external:W0
        )
    endif()
 
    target_compile_definitions(game PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
else()
    # GCC / clang++ (non-Windows or non-MSVC-frontend Clang)
    target_compile_options(game PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
endif()

target_compile_definitions(game PRIVATE VK_NO_PROTOTYPES)
target_link_libraries(game PRIVATE
    engine
    volk
)
