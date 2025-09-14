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

#define DESIRED_PRECISION (1e-10)

namespace test_matrix {
    constexpr auto range() -> Range<double> {
        return Range<double>{ -250000.0, 250000.0 };
    }

    auto random_vector(Random &rng) -> linalg::Vector3<double> {
        return linalg::Vector3<double>{
            rng.range(range()),
            rng.range(range()),
            rng.range(range())
        };
    }

    auto random_matrix(Random &rng) -> linalg::Matrix3<double> {
        return linalg::Matrix3<double>{
            rng.range(range()), rng.range(range()), rng.range(range()),
            rng.range(range()), rng.range(range()), rng.range(range()),
            rng.range(range()), rng.range(range()), rng.range(range()),
        };
    }

    TEST_CASE( "Matrix3LU::lower [Matrix3<double>]", "[Matrix3]" ) {
        SECTION("Single") {
            linalg::Matrix3LU<double> lu({
                1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0
            });

            CHECK_THAT( lu.lower().r1c1, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r1c2, Catch::Matchers::WithinAbs(0.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r1c3, Catch::Matchers::WithinAbs(0.0, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower().r2c1, Catch::Matchers::WithinAbs(4.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r2c2, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r2c3, Catch::Matchers::WithinAbs(0.0, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower().r3c1, Catch::Matchers::WithinAbs(7.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r3c2, Catch::Matchers::WithinAbs(8.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r3c3, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION) );
        }
    }

    TEST_CASE( "Matrix3LU::upper [Matrix3<double>]", "[Matrix3]" ) {
        SECTION("Single") {
            linalg::Matrix3LU<double> lu({
                1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0
            });

            CHECK_THAT( lu.upper().r1c1, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r1c2, Catch::Matchers::WithinAbs(2.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r1c3, Catch::Matchers::WithinAbs(3.0, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper().r2c1, Catch::Matchers::WithinAbs(0.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r2c2, Catch::Matchers::WithinAbs(5.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r2c3, Catch::Matchers::WithinAbs(6.0, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper().r3c1, Catch::Matchers::WithinAbs(0.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r3c2, Catch::Matchers::WithinAbs(0.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r3c3, Catch::Matchers::WithinAbs(9.0, DESIRED_PRECISION) );
        }
    }

    TEST_CASE( "Matrix3LU::from [Matrix3<double>]", "[Matrix3]" ) {
        SECTION("Single") {
            linalg::Matrix3<double> A(
                2.0, -1.0, -2.0,
                -4.0, 6.0, 3.0,
                -4.0, -2.0, 8.0
            );

            auto lu = linalg::Matrix3LU<double>::from(A);

            CHECK_THAT( lu.lower().r1c1, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r2c1, Catch::Matchers::WithinAbs(-2.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r3c1, Catch::Matchers::WithinAbs(-2.0, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower().r2c2, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.lower().r3c2, Catch::Matchers::WithinAbs(-1.0, DESIRED_PRECISION) );

            CHECK_THAT( lu.lower().r3c3, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION) );



            CHECK_THAT( lu.upper().r1c1, Catch::Matchers::WithinAbs(2.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r1c2, Catch::Matchers::WithinAbs(-1.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r1c3, Catch::Matchers::WithinAbs(-2.0, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper().r2c2, Catch::Matchers::WithinAbs(4.0, DESIRED_PRECISION) );
            CHECK_THAT( lu.upper().r2c3, Catch::Matchers::WithinAbs(-1.0, DESIRED_PRECISION) );

            CHECK_THAT( lu.upper().r3c3, Catch::Matchers::WithinAbs(3.0, DESIRED_PRECISION) );
        }
    }

    TEST_CASE( "blas2::matrix_vector_product [Matrix3<double>]", "[blas2][Matrix3]" ) {
        SECTION("Single") {
            linalg::Matrix3<double> A(
                2.0, 5.0, 1.0,
                1.0, 3.0, 0.5,
                6.0, 0.0, 2.5
            );
            linalg::Vector3<double> b(11.0, 6.0, -3.0);

            auto result = linalg::blas2::matrix_vector_product(A, b);

            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(49.0, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(27.5, DESIRED_PRECISION) );
            CHECK_THAT( result.z, Catch::Matchers::WithinAbs(58.5, DESIRED_PRECISION) );
        }
        Random rng;
        SECTION("Array") {
            std::vector<linalg::Matrix3<double>> A;
            std::vector<linalg::Vector3<double>> b;
            for (int i = 0; i < 100; i++) {
                A.push_back(random_matrix(rng));
                b.push_back(random_vector(rng));
            }

            for (size_t idx = 0; idx < A.size(); ++idx) {
                auto result = linalg::blas2::matrix_vector_product(A[idx], b[idx]);
                REQUIRE_THAT( result.x, Catch::Matchers::WithinAbs(A[idx].r1c1 * b[idx].x + A[idx].r1c2 * b[idx].y + A[idx].r1c3 * b[idx].z, DESIRED_PRECISION) );
                REQUIRE_THAT( result.y, Catch::Matchers::WithinAbs(A[idx].r2c1 * b[idx].x + A[idx].r2c2 * b[idx].y + A[idx].r2c3 * b[idx].z, DESIRED_PRECISION) );
                REQUIRE_THAT( result.z, Catch::Matchers::WithinAbs(A[idx].r3c1 * b[idx].x + A[idx].r3c2 * b[idx].y + A[idx].r3c3 * b[idx].z, DESIRED_PRECISION) );
            }
        }
    }

    TEST_CASE( "blas2::solve_lower_triangular [Matrix3<double>]", "[blas2][Matrix3]" ) {
        SECTION("Single") {
            linalg::Matrix3<double> A(
                2.0, 0.0, 0.0,
                1.0, 3.0, 0.0,
                6.0, 0.0, 2.5
            );
            linalg::Vector3<double> b(11.0, 6.0, -3.0);

            auto result = linalg::blas2::solve_lower_triangular(A, b);

            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(5.5, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(1.0 / 6.0, DESIRED_PRECISION) );
            CHECK_THAT( result.z, Catch::Matchers::WithinAbs(-14.4, DESIRED_PRECISION) );
        }
    }

    TEST_CASE( "blas2::solve_upper_triangular [Matrix3<double>]", "[blas2][Matrix3]" ) {
        SECTION("Single") {
            linalg::Matrix3<double> A(
                2.0, 5.0, 1.0,
                0.0, 3.0, 0.5,
                0.0, 0.0, 2.5
            );
            linalg::Vector3<double> b(11.0, 6.0, -3.0);

            auto result = linalg::blas2::solve_upper_triangular(A, b);

            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(0.6, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(2.2, DESIRED_PRECISION) );
            CHECK_THAT( result.z, Catch::Matchers::WithinAbs(-1.2, DESIRED_PRECISION) );
        }
    }

    TEST_CASE( "blas2::solve [Matrix3<double>]", "[blas2][Matrix3]" ) {
        SECTION("Single") {
            linalg::Matrix3<double> A(
                2.0, 5.0, 1.0,
                1.0, 3.0, 0.5,
                6.0, 0.0, 2.5
            );
            linalg::Vector3<double> b(11.0, 6.0, -3.0);

            auto result = linalg::blas2::solve(A, b);

            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(-18.0, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION) );
            CHECK_THAT( result.z, Catch::Matchers::WithinAbs(42.0, DESIRED_PRECISION) );
        }
    }
}