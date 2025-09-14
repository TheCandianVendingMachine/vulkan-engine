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

constexpr auto range() -> Range<float> {
    return Range<float>{ -250000.f, 250000.f };
}

auto random_vector(Random &rng) -> linalg::Vector2<float> {
    return linalg::Vector2<float>{
        rng.range(range()),
        rng.range(range())
    };
}

auto random_matrix(Random &rng) -> linalg::Matrix2<float> {
    return linalg::Matrix2<float>{
        rng.range(range()), rng.range(range()),
        rng.range(range()), rng.range(range())
    };
}


TEST_CASE( "Matrix2LU::lower [Matrix2<float>]", "[Matrix2]" ) {
    SECTION("non-unit") {
        linalg::Matrix2LU<float> lu({
            1.f, 2.f,
            3.f, 4.f,
        });

        CHECK_THAT( lu.lower().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.lower().r1c2, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION) );

        CHECK_THAT( lu.lower().r2c1, Catch::Matchers::WithinAbs(3.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.lower().r2c2, Catch::Matchers::WithinAbs(4.f, DESIRED_PRECISION) );
    }

    SECTION("unit") {
        linalg::Matrix2LU<float> lu({
            1.f, 2.f,
            3.f, 4.f,
        });

        CHECK_THAT( lu.lower_unit().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.lower_unit().r1c2, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION) );

        CHECK_THAT( lu.lower_unit().r2c1, Catch::Matchers::WithinAbs(3.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.lower_unit().r2c2, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
    }
}

TEST_CASE( "Matrix2LU::upper [Matrix2<float>]", "[Matrix2]" ) {
    SECTION("non-unit") {
        linalg::Matrix2LU<float> lu({
            1.f, 2.f,
            3.f, 4.f,
        });

        CHECK_THAT( lu.upper().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.upper().r1c2, Catch::Matchers::WithinAbs(2.f, DESIRED_PRECISION) );

        CHECK_THAT( lu.upper().r2c1, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.upper().r2c2, Catch::Matchers::WithinAbs(4.f, DESIRED_PRECISION) );
    }

    SECTION("unit") {
        linalg::Matrix2LU<float> lu({
            1.f, 2.f,
            3.f, 4.f,
        });

        CHECK_THAT( lu.upper_unit().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.upper_unit().r1c2, Catch::Matchers::WithinAbs(2.f, DESIRED_PRECISION) );

        CHECK_THAT( lu.upper_unit().r2c1, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.upper_unit().r2c2, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
    }
}


TEST_CASE( "Matrix2LU::from [Matrix2<float>]", "[Matrix2]" ) {
    SECTION("Single") {
        linalg::Matrix2<float> A(
            2.f, 5.f,
            1.f, 3.f
        );

        auto lu = linalg::Matrix2LU<float>::from(A);

        CHECK_THAT( lu.lower_unit().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.lower_unit().r1c2, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.lower_unit().r2c1, Catch::Matchers::WithinAbs(0.5f, DESIRED_PRECISION) );
        CHECK_THAT( lu.lower_unit().r2c2, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );

        CHECK_THAT( lu.upper().r1c1, Catch::Matchers::WithinAbs(2.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.upper().r1c2, Catch::Matchers::WithinAbs(5.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.upper().r2c1, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION) );
        CHECK_THAT( lu.upper().r2c2, Catch::Matchers::WithinAbs(0.5f, DESIRED_PRECISION) );
    }
}

TEST_CASE( "blas2::matrix_vector_product [Matrix2<float>]", "[blas2][Matrix2]" ) {
    SECTION("Single") {
        linalg::Matrix2<float> A(
            2.f, 5.f,
            1.f, 3.f
        );
        linalg::Vector2<float> b(11.f, 6.f);

        auto result = linalg::blas2::matrix_vector_product(A, b);

        CHECK_THAT( result.x, Catch::Matchers::WithinAbs(52.f, DESIRED_PRECISION) );
        CHECK_THAT( result.y, Catch::Matchers::WithinAbs(29.f, DESIRED_PRECISION) );
    }
    Random rng;
    SECTION("Array") {
        std::vector<linalg::Matrix2<float>> A;
        std::vector<linalg::Vector2<float>> b;
        for (int i = 0; i < 100; i++) {
            A.push_back(random_matrix(rng));
            b.push_back(random_vector(rng));
        }

        for (size_t idx = 0; idx < A.size(); ++idx) {
            auto result = linalg::blas2::matrix_vector_product(A[idx], b[idx]);
            REQUIRE_THAT( result.x, Catch::Matchers::WithinAbs(A[idx].r1c1 * b[idx].x + A[idx].r1c2 * b[idx].y, DESIRED_PRECISION) );
            REQUIRE_THAT( result.y, Catch::Matchers::WithinAbs(A[idx].r2c1 * b[idx].x + A[idx].r2c2 * b[idx].y, DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas2::solve_lower_triangular [Matrix2<float>]", "[blas2][Matrix2]" ) {
    SECTION("Single") {
        linalg::Matrix2<float> A(
            2.f, 0.f,
            1.f, 3.f
        );
        linalg::Vector2<float> b(11.f, 6.f);

        auto result = linalg::blas2::solve_lower_triangular(A, b);

        CHECK_THAT( result.x, Catch::Matchers::WithinAbs(11.f / 2.f, DESIRED_PRECISION) );
        CHECK_THAT( result.y, Catch::Matchers::WithinAbs(1.f / 6.f, DESIRED_PRECISION) );
    }
}

TEST_CASE( "blas2::solve_upper_triangular [Matrix2<float>]", "[blas2][Matrix2]" ) {
    SECTION("Single") {
        linalg::Matrix2<float> A(
            2.f, 5.f,
            0.f, 3.f
        );
        linalg::Vector2<float> b(11.f, 6.f);

        auto result = linalg::blas2::solve_upper_triangular(A, b);

        CHECK_THAT( result.x, Catch::Matchers::WithinAbs(0.5f, DESIRED_PRECISION) );
        CHECK_THAT( result.y, Catch::Matchers::WithinAbs(2.f, DESIRED_PRECISION) );
    }
}

TEST_CASE( "blas2::solve [Matrix2<float>]", "[blas2][Matrix2]" ) {
    SECTION("Single") {
        linalg::Matrix2<float> A(
            2.f, 5.f,
            1.f, 3.f
        );
        linalg::Vector2<float> b(11.f, 6.f);

        auto result = linalg::blas2::solve(A, b);

        CHECK_THAT( result.x, Catch::Matchers::WithinAbs(3.f, DESIRED_PRECISION) );
        CHECK_THAT( result.y, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION) );
    }
}