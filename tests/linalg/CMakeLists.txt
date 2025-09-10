enable_testing()

add_executable(test_linalg_scalar
    test_vector2_float.cpp
    test_vector3_float.cpp
    test_vector4_float.cpp
    test_vector2_double.cpp
    test_vector3_double.cpp
    test_vector4_double.cpp)
target_include_directories(test_linalg_scalar PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${EXTERNAL_DIR}/catch2/include
)
target_link_libraries(test_linalg_scalar PRIVATE
    Catch2::Catch2WithMain
    engine
    linalg_scalar
)

add_executable(test_linalg_sse test_vector2_float.cpp)
target_include_directories(test_linalg_sse PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${EXTERNAL_DIR}/catch2/include
)
target_link_libraries(test_linalg_sse PRIVATE
    Catch2::Catch2WithMain
    engine
    linalg_sse
)

catch_discover_tests(test_linalg_scalar
    DL_PATHS "${CMAKE_BINARY_DIR}/linalg/Debug" "${CMAKE_BINARY_DIR}/linalg/Release")
catch_discover_tests(test_linalg_sse
    DL_PATHS "${CMAKE_BINARY_DIR}/linalg/Debug" "${CMAKE_BINARY_DIR}/linalg/Release")
