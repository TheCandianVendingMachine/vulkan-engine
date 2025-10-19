#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <vector>

#include <engine/random.h>

#include <linalg/matrix.h>
#include <linalg/matrix_ops.h>
#include <linalg/vector.h>

#define DESIRED_PRECISION (1e-10)

constexpr auto range() -> Range<double> { return Range<double>{-250000.0, 250000.0}; }

auto random_vector(Random& rng) -> linalg::Vector2<double> { return linalg::Vector2<double>{rng.range(range()), rng.range(range())}; }

auto random_matrix(Random& rng) -> linalg::Matrix2<double> {
    return linalg::Matrix2<double>{rng.range(range()), rng.range(range()), rng.range(range()), rng.range(range())};
}

TEST_CASE("Matrix2LU::lower [Matrix2<double>]", "[Matrix2]") {
    SECTION("unit") {
        linalg::Matrix2LU<double> lu({
            1.0,
            2.f,
            3.0,
            4.f,
        });

        CHECK_THAT(lu.lower_unit().r1c1, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r1c2, Catch::Matchers::WithinAbs(0.0, DESIRED_PRECISION));

        CHECK_THAT(lu.lower_unit().r2c1, Catch::Matchers::WithinAbs(3.0, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r2c2, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION));
    }

    SECTION("non-unit") {
        linalg::Matrix2LU<double> lu({
            1.0,
            2.f,
            3.0,
            4.f,
        });

        CHECK_THAT(lu.lower().r1c1, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION));
        CHECK_THAT(lu.lower().r1c2, Catch::Matchers::WithinAbs(0.0, DESIRED_PRECISION));

        CHECK_THAT(lu.lower().r2c1, Catch::Matchers::WithinAbs(3.0, DESIRED_PRECISION));
        CHECK_THAT(lu.lower().r2c2, Catch::Matchers::WithinAbs(4.0, DESIRED_PRECISION));
    }
}

TEST_CASE("Matrix2LU::upper [Matrix2<double>]", "[Matrix2]") {
    SECTION("unit") {
        linalg::Matrix2LU<double> lu({
            1.0,
            2.f,
            3.0,
            4.f,
        });

        CHECK_THAT(lu.upper_unit().r1c1, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION));
        CHECK_THAT(lu.upper_unit().r1c2, Catch::Matchers::WithinAbs(2.0, DESIRED_PRECISION));

        CHECK_THAT(lu.upper_unit().r2c1, Catch::Matchers::WithinAbs(0.0, DESIRED_PRECISION));
        CHECK_THAT(lu.upper_unit().r2c2, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION));
    }

    SECTION("non-unit") {
        linalg::Matrix2LU<double> lu({
            1.0,
            2.f,
            3.0,
            4.f,
        });

        CHECK_THAT(lu.upper().r1c1, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r1c2, Catch::Matchers::WithinAbs(2.0, DESIRED_PRECISION));

        CHECK_THAT(lu.upper().r2c1, Catch::Matchers::WithinAbs(0.0, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r2c2, Catch::Matchers::WithinAbs(4.0, DESIRED_PRECISION));
    }
}

TEST_CASE("Matrix2LU::from [Matrix2<double>]", "[Matrix2]") {
    SECTION("Single") {
        linalg::Matrix2<double> A(2.0, 5.0, 1.0, 3.0);

        auto lu = linalg::Matrix2LU<double>::from(A);

        CHECK_THAT(lu.lower_unit().r1c1, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r2c1, Catch::Matchers::WithinAbs(0.5, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r2c2, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION));

        CHECK_THAT(lu.upper().r1c1, Catch::Matchers::WithinAbs(2.0, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r1c2, Catch::Matchers::WithinAbs(5.0, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r2c2, Catch::Matchers::WithinAbs(0.5, DESIRED_PRECISION));
    }
}

TEST_CASE("blas2::matrix_vector_product [Matrix2<double>]", "[blas2][Matrix2]") {
    SECTION("Single") {
        linalg::Matrix2<double> A(2.0, 5.0, 1.0, 3.0);
        linalg::Vector2<double> b(11.0, 6.0);

        auto result = linalg::blas2::matrix_vector_product(A, b);

        CHECK_THAT(result.x, Catch::Matchers::WithinAbs(52.0, DESIRED_PRECISION));
        CHECK_THAT(result.y, Catch::Matchers::WithinAbs(29.0, DESIRED_PRECISION));
    }
    Random rng;
    SECTION("Array") {
        std::vector<linalg::Matrix2<double>> A;
        std::vector<linalg::Vector2<double>> b;
        for (int i = 0; i < 100; i++) {
            A.push_back(random_matrix(rng));
            b.push_back(random_vector(rng));
        }

        for (size_t idx = 0; idx < A.size(); ++idx) {
            auto result = linalg::blas2::matrix_vector_product(A[idx], b[idx]);
            REQUIRE_THAT(result.x, Catch::Matchers::WithinAbs(A[idx].r1c1 * b[idx].x + A[idx].r1c2 * b[idx].y, DESIRED_PRECISION));
            REQUIRE_THAT(result.y, Catch::Matchers::WithinAbs(A[idx].r2c1 * b[idx].x + A[idx].r2c2 * b[idx].y, DESIRED_PRECISION));
        }
    }
}

TEST_CASE("blas2::solve_lower_triangular [Matrix2<double>]", "[blas2][Matrix2]") {
    SECTION("Single") {
        linalg::Matrix2<double> A(2.0, 0.0, 1.0, 3.0);
        linalg::Vector2<double> b(11.0, 6.0);

        auto result = linalg::blas2::solve_lower_triangular(A, b);

        CHECK_THAT(result.x, Catch::Matchers::WithinAbs(11.0 / 2.0, DESIRED_PRECISION));
        CHECK_THAT(result.y, Catch::Matchers::WithinAbs(1.0 / 6.0, DESIRED_PRECISION));
    }
}

TEST_CASE("blas2::solve_upper_triangular [Matrix2<double>]", "[blas2][Matrix2]") {
    SECTION("Single") {
        linalg::Matrix2<double> A(2.0, 5.0, 0.0, 3.0);
        linalg::Vector2<double> b(11.0, 6.0);

        auto result = linalg::blas2::solve_upper_triangular(A, b);

        CHECK_THAT(result.x, Catch::Matchers::WithinAbs(0.5, DESIRED_PRECISION));
        CHECK_THAT(result.y, Catch::Matchers::WithinAbs(2.0, DESIRED_PRECISION));
    }
}

TEST_CASE("blas2::solve [Matrix2<double>]", "[blas2][Matrix2]") {
    SECTION("Single") {
        linalg::Matrix2<double> A(2.0, 5.0, 1.0, 3.0);
        linalg::Vector2<double> b(11.0, 6.0);

        auto result = linalg::blas2::solve(A, b);

        CHECK_THAT(result.x, Catch::Matchers::WithinAbs(3.0, DESIRED_PRECISION));
        CHECK_THAT(result.y, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION));
    }
}