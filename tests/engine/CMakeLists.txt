enable_testing()

add_executable(test_engine
    test_bitset.cpp
    test_region.cpp
    test_pool.cpp
    )
target_include_directories(test_engine PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${EXTERNAL_DIR}/catch2/include
)
target_link_libraries(test_engine PRIVATE
    Catch2::Catch2WithMain
    engine
    linalg_scalar
)
target_compile_features(test_engine PRIVATE cxx_std_23)

if (MSVC)
    target_compile_options(test_engine PRIVATE
        /utf-8
    )
    target_compile_definitions(test_engine PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()


catch_discover_tests(test_engine
    DL_PATHS "${CMAKE_BINARY_DIR}/engine/Release" "${CMAKE_BINARY_DIR}/engine/Debug")
