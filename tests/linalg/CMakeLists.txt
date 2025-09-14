enable_testing()

add_executable(test_linalg_scalar
    test_vector2_float.cpp
    test_vector3_float.cpp
    test_vector4_float.cpp
    test_vector2_double.cpp
    test_vector3_double.cpp
    test_vector4_double.cpp
    test_matrix2_float.cpp
    test_matrix2_double.cpp
    test_matrix3_float.cpp
    test_matrix3_double.cpp
    test_matrix4_float.cpp
    test_matrix4_double.cpp)
target_include_directories(test_linalg_scalar PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${EXTERNAL_DIR}/catch2/include
)
target_link_libraries(test_linalg_scalar PRIVATE
    Catch2::Catch2WithMain
    engine
    linalg_scalar
)
target_compile_features(test_linalg_scalar PRIVATE cxx_std_23)

add_executable(test_linalg_sse
    test_vector2_float.cpp
    test_vector3_float.cpp
    test_vector4_float.cpp
    test_vector2_double.cpp
    test_vector3_double.cpp
    test_vector4_double.cpp)
target_include_directories(test_linalg_sse PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${EXTERNAL_DIR}/catch2/include
)
target_link_libraries(test_linalg_sse PRIVATE
    Catch2::Catch2WithMain
    engine
    linalg_sse
)
target_compile_features(test_linalg_sse PRIVATE cxx_std_23)


add_executable(bench_linalg_scalar
    bench_vector2_float.cpp
    bench_vector3_float.cpp
    bench_vector4_float.cpp
    bench_vector2_double.cpp
    bench_vector3_double.cpp
    bench_vector4_double.cpp
    bench_matrix2_float.cpp
    bench_matrix2_double.cpp
    bench_matrix3_float.cpp
    bench_matrix3_double.cpp
    bench_matrix4_float.cpp
    bench_matrix4_double.cpp)
target_include_directories(bench_linalg_scalar PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${EXTERNAL_DIR}/catch2/include
)
target_link_libraries(bench_linalg_scalar PRIVATE
    Catch2::Catch2WithMain
    engine
    linalg_scalar
)
target_compile_features(bench_linalg_scalar PRIVATE cxx_std_23)

add_executable(bench_linalg_sse
    bench_vector2_float.cpp
    bench_vector3_float.cpp
    bench_vector4_float.cpp
    bench_vector2_double.cpp
    bench_vector3_double.cpp
    bench_vector4_double.cpp)
target_include_directories(bench_linalg_sse PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${EXTERNAL_DIR}/catch2/include
)
target_link_libraries(bench_linalg_sse PRIVATE
    Catch2::Catch2WithMain
    engine
    linalg_sse
)
target_compile_features(bench_linalg_sse PRIVATE cxx_std_23)


catch_discover_tests(test_linalg_scalar
    DL_PATHS "${CMAKE_BINARY_DIR}/linalg/Release")
catch_discover_tests(test_linalg_sse
    DL_PATHS "${CMAKE_BINARY_DIR}/linalg/Release")

catch_discover_tests(bench_linalg_scalar
    DL_PATHS "${CMAKE_BINARY_DIR}/linalg/Release")
catch_discover_tests(bench_linalg_sse
    DL_PATHS "${CMAKE_BINARY_DIR}/linalg/Release")