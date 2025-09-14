#include <vector>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <engine/random.h>

#include <linalg/vector.h>
#include <linalg/matrix.h>
#include <linalg/matrix_ops.h>

#define DESIRED_PRECISION (1e-5)

namespace test_matrix {
    constexpr auto range() -> Range<float> {
        return Range<float>{ -250000.f, 250000.f };
    }

    auto random_vector(Random &rng) -> linalg::Vector4<float> {
        return linalg::Vector4<float>{
            rng.range(range()),
            rng.range(range()),
            rng.range(range()),
            rng.range(range()),
        };
    }

    auto random_matrix(Random &rng) -> linalg::Matrix4<float> {
        return linalg::Matrix4<float>{
            rng.range(range()), rng.range(range()), rng.range(range()), rng.range(range()),
            rng.range(range()), rng.range(range()), rng.range(range()), rng.range(range()),
            rng.range(range()), rng.range(range()), rng.range(range()), rng.range(range()),
            rng.range(range()), rng.range(range()), rng.range(range()), rng.range(range()),
        };
    }

    TEST_CASE( "Matrix4LU::lower [Matrix4<float>]", "[Matrix4]" ) {
        SECTION("unit") {
            linalg::Matrix4LU<float> lu({
                1.0f, 2.0f, 3.0f, 4.0f,
                5.0f, 6.0f, 7.0f, 8.0f,
                9.0f, 10.0f, 11.0f, 12.0f,
                13.0f, 14.0f, 15.0f, 16.0f
            });

            CHECK_THAT( lu.lower_unit().r1c1, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r1c2, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r1c3, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r1c4, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower_unit().r2c1, Catch::Matchers::WithinAbs(5.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r2c2, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r2c3, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r2c4, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower_unit().r3c1, Catch::Matchers::WithinAbs(9.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r3c2, Catch::Matchers::WithinAbs(10.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r3c3, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r3c4, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower_unit().r4c1, Catch::Matchers::WithinAbs(13.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r4c2, Catch::Matchers::WithinAbs(14.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r4c3, Catch::Matchers::WithinAbs(15.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower_unit().r4c4, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
        }

        SECTION("non-unit") {
            linalg::Matrix4LU<float> lu({
                1.0f, 2.0f, 3.0f, 4.0f,
                5.0f, 6.0f, 7.0f, 8.0f,
                9.0f, 10.0f, 11.0f, 12.0f,
                13.0f, 14.0f, 15.0f, 16.0f
            });

            CHECK_THAT( lu.lower().r1c1, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r1c2, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r1c3, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r1c4, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower().r2c1, Catch::Matchers::WithinAbs(5.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r2c2, Catch::Matchers::WithinAbs(6.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r2c3, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r2c4, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower().r3c1, Catch::Matchers::WithinAbs(9.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r3c2, Catch::Matchers::WithinAbs(10.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r3c3, Catch::Matchers::WithinAbs(11.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r3c4, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower().r4c1, Catch::Matchers::WithinAbs(13.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r4c2, Catch::Matchers::WithinAbs(14.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r4c3, Catch::Matchers::WithinAbs(15.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r4c4, Catch::Matchers::WithinAbs(16.0f, DESIRED_PRECISION) );
        }
    }

    TEST_CASE( "Matrix4LU::upper [Matrix4<float>]", "[Matrix4]" ) {
        SECTION("unit") {
            linalg::Matrix4LU<float> lu({
                1.0f, 2.0f, 3.0f, 4.0f,
                5.0f, 6.0f, 7.0f, 8.0f,
                9.0f, 10.0f, 11.0f, 12.0f,
                13.0f, 14.0f, 15.0f, 16.0f
            });

            CHECK_THAT( lu.upper_unit().r1c1, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r1c2, Catch::Matchers::WithinAbs(2.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r1c3, Catch::Matchers::WithinAbs(3.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r1c4, Catch::Matchers::WithinAbs(4.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper_unit().r2c1, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r2c2, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r2c3, Catch::Matchers::WithinAbs(7.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r2c4, Catch::Matchers::WithinAbs(8.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper_unit().r3c1, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r3c2, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r3c3, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r3c4, Catch::Matchers::WithinAbs(12.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper_unit().r4c1, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r4c2, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r4c3, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r4c4, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
        }

        SECTION("non-unit") {
            linalg::Matrix4LU<float> lu({
                1.0f, 2.0f, 3.0f, 4.0f,
                5.0f, 6.0f, 7.0f, 8.0f,
                9.0f, 10.0f, 11.0f, 12.0f,
                13.0f, 14.0f, 15.0f, 16.0f
            });

            CHECK_THAT( lu.upper().r1c1, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r1c2, Catch::Matchers::WithinAbs(2.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r1c3, Catch::Matchers::WithinAbs(3.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r1c4, Catch::Matchers::WithinAbs(4.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper().r2c1, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r2c2, Catch::Matchers::WithinAbs(6.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r2c3, Catch::Matchers::WithinAbs(7.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r2c4, Catch::Matchers::WithinAbs(8.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper().r3c1, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r3c2, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r3c3, Catch::Matchers::WithinAbs(11.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r3c4, Catch::Matchers::WithinAbs(12.0f, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper().r4c1, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r4c2, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r4c3, Catch::Matchers::WithinAbs(0.0f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r4c4, Catch::Matchers::WithinAbs(16.0f, DESIRED_PRECISION) );
        }
    }

    TEST_CASE( "Matrix4LU::from [Matrix4<float>]", "[Matrix4]" ) {
        SECTION("Single") {
            linalg::Matrix4<float> A(
                2.f, -1.f, -2.f, 9.f,
                -4.f, 6.f, 3.f, -5.f,
                -4.f, -2.f, 8.f, 0.f,
                2.f, 1.f, 1.f, -1.f
            );

            auto lu = linalg::Matrix4LU<float>::from(A);

            CHECK_THAT( lu.lower().r1c1, Catch::Matchers::WithinAbs(2.f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r2c1, Catch::Matchers::WithinAbs(-4.f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r3c1, Catch::Matchers::WithinAbs(-4.f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r4c1, Catch::Matchers::WithinAbs(2.f, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower().r2c2, Catch::Matchers::WithinAbs(4.f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r3c2, Catch::Matchers::WithinAbs(-4.f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r4c2, Catch::Matchers::WithinAbs(2.f, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower().r3c3, Catch::Matchers::WithinAbs(3.f, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r4c3, Catch::Matchers::WithinAbs(3.5f, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower().r4c4, Catch::Matchers::WithinAbs(-158.f / 3.f, DESIRED_PRECISION) );
            

            CHECK_THAT( lu.upper_unit().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r1c2, Catch::Matchers::WithinAbs(-0.5f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r1c3, Catch::Matchers::WithinAbs(-1.f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r1c4, Catch::Matchers::WithinAbs(4.5f, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper_unit().r2c2, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r2c3, Catch::Matchers::WithinAbs(-0.25f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r2c4, Catch::Matchers::WithinAbs(3.25f, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper_unit().r3c3, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper_unit().r3c4, Catch::Matchers::WithinAbs(31.f / 3.f, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper_unit().r4c4, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
        }
    }

    TEST_CASE( "blas2::matrix_vector_product [Matrix4<float>]", "[blas2][Matrix4]" ) {
        SECTION("Single") {
            linalg::Matrix4<float> A(
                2.f, -1.f, -2.f, 9.f,
                -4.f, 6.f, 3.f, -5.f,
                -4.f, -2.f, 8.f, 0.f,
                2.f, 1.f, 1.f, -1.f
            );
            linalg::Vector4<float> b(11.f, 6.f, -3.f, 1.f);

            auto result = linalg::blas2::matrix_vector_product(A, b);

            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(31.f, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(-22.f, DESIRED_PRECISION) );
            CHECK_THAT( result.z, Catch::Matchers::WithinAbs(-80.f, DESIRED_PRECISION) );
            CHECK_THAT( result.w, Catch::Matchers::WithinAbs(24.f, DESIRED_PRECISION) );
        }
        Random rng;
        SECTION("Array") {
            std::vector<linalg::Matrix4<float>> A;
            std::vector<linalg::Vector4<float>> b;
            for (int i = 0; i < 100; i++) {
                A.push_back(random_matrix(rng));
                b.push_back(random_vector(rng));
            }

            for (size_t idx = 0; idx < A.size(); ++idx) {
                auto result = linalg::blas2::matrix_vector_product(A[idx], b[idx]);
                REQUIRE_THAT( result.x, Catch::Matchers::WithinAbs(A[idx].r1c1 * b[idx].x + A[idx].r1c2 * b[idx].y + A[idx].r1c3 * b[idx].z + A[idx].r1c4 * b[idx].w, DESIRED_PRECISION) );
                REQUIRE_THAT( result.y, Catch::Matchers::WithinAbs(A[idx].r2c1 * b[idx].x + A[idx].r2c2 * b[idx].y + A[idx].r2c3 * b[idx].z + A[idx].r2c4 * b[idx].w, DESIRED_PRECISION) );
                REQUIRE_THAT( result.z, Catch::Matchers::WithinAbs(A[idx].r3c1 * b[idx].x + A[idx].r3c2 * b[idx].y + A[idx].r3c3 * b[idx].z + A[idx].r3c4 * b[idx].w, DESIRED_PRECISION) );
                REQUIRE_THAT( result.w, Catch::Matchers::WithinAbs(A[idx].r4c1 * b[idx].x + A[idx].r4c2 * b[idx].y + A[idx].r4c3 * b[idx].z + A[idx].r4c4 * b[idx].w, DESIRED_PRECISION) );
            }
        }
    }

    TEST_CASE( "blas2::solve_lower_triangular [Matrix4<float>]", "[blas2][Matrix4]" ) {
        SECTION("Single") {
            linalg::Matrix4<float> A(
                2.f, 0.f, 0.f, 0.f,
                -4.f, 6.f, 0.f, 0.f,
                -4.f, -2.f, 8.f, 0.f,
                2.f, 1.f, 1.f, -1.f
            );
            linalg::Vector4<float> b(11.f, 6.f, -3.f, 1.f);

            auto result = linalg::blas2::solve_lower_triangular(A, b);

            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(5.5f, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(14.f / 3.f, DESIRED_PRECISION) );
            CHECK_THAT( result.z, Catch::Matchers::WithinAbs(85.f / 24.f, DESIRED_PRECISION) );
            CHECK_THAT( result.w, Catch::Matchers::WithinAbs(437.f / 24.f, DESIRED_PRECISION) );
        }
    }

    TEST_CASE( "blas2::solve_upper_triangular [Matrix4<float>]", "[blas2][Matrix4]" ) {
        SECTION("Single") {
            linalg::Matrix4<float> A(
                2.f, -1.f, -2.f, 9.f,
                0.f, 6.f, 3.f, -5.f,
                0.f, 0.f, 8.f, 0.f,
                0.f, 0.f, 0.f, -1.f
            );
            linalg::Vector4<float> b(11.f, 6.f, -3.f, 1.f);

            auto result = linalg::blas2::solve_upper_triangular(A, b);

            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(941.f / 96.f, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(17.f / 48.f, DESIRED_PRECISION) );
            CHECK_THAT( result.z, Catch::Matchers::WithinAbs(-3.f / 8.f, DESIRED_PRECISION) );
            CHECK_THAT( result.w, Catch::Matchers::WithinAbs(-1.f, DESIRED_PRECISION) );
        }
    }

    TEST_CASE( "blas2::solve [Matrix4<float>]", "[blas2][Matrix4]" ) {
        SECTION("Single") {
            linalg::Matrix4<float> A(
                2.f, -1.f, -2.f, 9.f,
                -4.f, 6.f, 3.f, -5.f,
                -4.f, -2.f, 8.f, 0.f,
                2.f, 1.f, 1.f, -1.f
            );
            linalg::Vector4<float> b(11.f, 6.f, -3.f, 1.f);

            auto result = linalg::blas2::solve(A, b);

            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(71.f / 1264.f, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(1381.f / 632.f, DESIRED_PRECISION) );
            CHECK_THAT( result.z, Catch::Matchers::WithinAbs(63.f / 316.f, DESIRED_PRECISION) );
            CHECK_THAT( result.w, Catch::Matchers::WithinAbs(473.f / 316.f, DESIRED_PRECISION) );
        }
    }
}