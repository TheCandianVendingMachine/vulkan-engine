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

namespace test_matrix {
constexpr auto range() -> Range<float> { return Range<float>{-250000.f, 250000.f}; }

auto random_vector(Random& rng) -> linalg::Vector3<float> {
    return linalg::Vector3<float>{rng.range(range()), rng.range(range()), rng.range(range())};
}

auto random_matrix(Random& rng) -> linalg::Matrix3<float> {
    return linalg::Matrix3<float>{
        rng.range(range()), rng.range(range()), rng.range(range()), rng.range(range()), rng.range(range()),
        rng.range(range()), rng.range(range()), rng.range(range()), rng.range(range()),
    };
}

TEST_CASE("Matrix3LU::lower [Matrix3<float>]", "[Matrix3]") {
    SECTION("unit") {
        linalg::Matrix3LU<float> lu({1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f});

        CHECK_THAT(lu.lower_unit().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r1c2, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r1c3, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));

        CHECK_THAT(lu.lower_unit().r2c1, Catch::Matchers::WithinAbs(4.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r2c2, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r2c3, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));

        CHECK_THAT(lu.lower_unit().r3c1, Catch::Matchers::WithinAbs(7.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r3c2, Catch::Matchers::WithinAbs(8.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r3c3, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
    }

    SECTION("non-unit") {
        linalg::Matrix3LU<float> lu({1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f});

        CHECK_THAT(lu.lower().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower().r1c2, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower().r1c3, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));

        CHECK_THAT(lu.lower().r2c1, Catch::Matchers::WithinAbs(4.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower().r2c2, Catch::Matchers::WithinAbs(5.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower().r2c3, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));

        CHECK_THAT(lu.lower().r3c1, Catch::Matchers::WithinAbs(7.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower().r3c2, Catch::Matchers::WithinAbs(8.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower().r3c3, Catch::Matchers::WithinAbs(9.f, DESIRED_PRECISION));
    }
}

TEST_CASE("Matrix3LU::upper [Matrix3<float>]", "[Matrix3]") {
    SECTION("unit") {
        linalg::Matrix3LU<float> lu({1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f});

        CHECK_THAT(lu.upper_unit().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper_unit().r1c2, Catch::Matchers::WithinAbs(2.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper_unit().r1c3, Catch::Matchers::WithinAbs(3.f, DESIRED_PRECISION));

        CHECK_THAT(lu.upper_unit().r2c1, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper_unit().r2c2, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper_unit().r2c3, Catch::Matchers::WithinAbs(6.f, DESIRED_PRECISION));

        CHECK_THAT(lu.upper_unit().r3c1, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper_unit().r3c2, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper_unit().r3c3, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
    }

    SECTION("non-unit") {
        linalg::Matrix3LU<float> lu({1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f});

        CHECK_THAT(lu.upper().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r1c2, Catch::Matchers::WithinAbs(2.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r1c3, Catch::Matchers::WithinAbs(3.f, DESIRED_PRECISION));

        CHECK_THAT(lu.upper().r2c1, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r2c2, Catch::Matchers::WithinAbs(5.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r2c3, Catch::Matchers::WithinAbs(6.f, DESIRED_PRECISION));

        CHECK_THAT(lu.upper().r3c1, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r3c2, Catch::Matchers::WithinAbs(0.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r3c3, Catch::Matchers::WithinAbs(9.f, DESIRED_PRECISION));
    }
}

TEST_CASE("Matrix3LU::from [Matrix3<float>]", "[Matrix3]") {
    SECTION("Single") {
        linalg::Matrix3<float> A(2.f, -1.f, -2.f, -4.f, 6.f, 3.f, -4.f, -2.f, 8.f);

        auto lu = linalg::Matrix3LU<float>::from(A);

        CHECK_THAT(lu.lower_unit().r1c1, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r2c1, Catch::Matchers::WithinAbs(-2.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r3c1, Catch::Matchers::WithinAbs(-2.f, DESIRED_PRECISION));

        CHECK_THAT(lu.lower_unit().r2c2, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
        CHECK_THAT(lu.lower_unit().r3c2, Catch::Matchers::WithinAbs(-1.f, DESIRED_PRECISION));

        CHECK_THAT(lu.lower_unit().r3c3, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));

        CHECK_THAT(lu.upper().r1c1, Catch::Matchers::WithinAbs(2.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r1c2, Catch::Matchers::WithinAbs(-1.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r1c3, Catch::Matchers::WithinAbs(-2.f, DESIRED_PRECISION));

        CHECK_THAT(lu.upper().r2c2, Catch::Matchers::WithinAbs(4.f, DESIRED_PRECISION));
        CHECK_THAT(lu.upper().r2c3, Catch::Matchers::WithinAbs(-1.f, DESIRED_PRECISION));

        CHECK_THAT(lu.upper().r3c3, Catch::Matchers::WithinAbs(3.f, DESIRED_PRECISION));
    }
}

TEST_CASE("blas2::matrix_vector_product [Matrix3<float>]", "[blas2][Matrix3]") {
    SECTION("Single") {
        linalg::Matrix3<float> A(2.f, 5.f, 1.f, 1.f, 3.f, 0.5f, 6.f, 0.f, 2.5f);
        linalg::Vector3<float> b(11.f, 6.f, -3.f);

        auto result = linalg::blas2::matrix_vector_product(A, b);

        CHECK_THAT(result.x, Catch::Matchers::WithinAbs(49.f, DESIRED_PRECISION));
        CHECK_THAT(result.y, Catch::Matchers::WithinAbs(27.5f, DESIRED_PRECISION));
        CHECK_THAT(result.z, Catch::Matchers::WithinAbs(58.5f, DESIRED_PRECISION));
    }
    Random rng;
    SECTION("Array") {
        std::vector<linalg::Matrix3<float>> A;
        std::vector<linalg::Vector3<float>> b;
        for (int i = 0; i < 100; i++) {
            A.push_back(random_matrix(rng));
            b.push_back(random_vector(rng));
        }

        for (size_t idx = 0; idx < A.size(); ++idx) {
            auto result = linalg::blas2::matrix_vector_product(A[idx], b[idx]);
            REQUIRE_THAT(result.x, Catch::Matchers::WithinAbs(A[idx].r1c1 * b[idx].x + A[idx].r1c2 * b[idx].y + A[idx].r1c3 * b[idx].z,
                                                              DESIRED_PRECISION));
            REQUIRE_THAT(result.y, Catch::Matchers::WithinAbs(A[idx].r2c1 * b[idx].x + A[idx].r2c2 * b[idx].y + A[idx].r2c3 * b[idx].z,
                                                              DESIRED_PRECISION));
            REQUIRE_THAT(result.z, Catch::Matchers::WithinAbs(A[idx].r3c1 * b[idx].x + A[idx].r3c2 * b[idx].y + A[idx].r3c3 * b[idx].z,
                                                              DESIRED_PRECISION));
        }
    }
}

TEST_CASE("blas2::solve_lower_triangular [Matrix3<float>]", "[blas2][Matrix3]") {
    SECTION("Single") {
        linalg::Matrix3<float> A(2.f, 0.f, 0.f, 1.f, 3.f, 0.f, 6.f, 0.f, 2.5f);
        linalg::Vector3<float> b(11.f, 6.f, -3.f);

        auto result = linalg::blas2::solve_lower_triangular(A, b);

        CHECK_THAT(result.x, Catch::Matchers::WithinAbs(5.5f, DESIRED_PRECISION));
        CHECK_THAT(result.y, Catch::Matchers::WithinAbs(1.f / 6.f, DESIRED_PRECISION));
        CHECK_THAT(result.z, Catch::Matchers::WithinAbs(-14.4f, DESIRED_PRECISION));
    }
}

TEST_CASE("blas2::solve_upper_triangular [Matrix3<float>]", "[blas2][Matrix3]") {
    SECTION("Single") {
        linalg::Matrix3<float> A(2.f, 5.f, 1.f, 0.f, 3.f, 0.5f, 0.f, 0.f, 2.5f);
        linalg::Vector3<float> b(11.f, 6.f, -3.f);

        auto result = linalg::blas2::solve_upper_triangular(A, b);

        CHECK_THAT(result.x, Catch::Matchers::WithinAbs(0.6f, DESIRED_PRECISION));
        CHECK_THAT(result.y, Catch::Matchers::WithinAbs(2.2f, DESIRED_PRECISION));
        CHECK_THAT(result.z, Catch::Matchers::WithinAbs(-1.2f, DESIRED_PRECISION));
    }
}

TEST_CASE("blas2::solve [Matrix3<float>]", "[blas2][Matrix3]") {
    SECTION("Single") {
        linalg::Matrix3<float> A(2.f, 5.f, 1.f, 1.f, 3.f, 0.5f, 6.f, 0.f, 2.5f);
        linalg::Vector3<float> b(11.f, 6.f, -3.f);

        auto result = linalg::blas2::solve(A, b);

        CHECK_THAT(result.x, Catch::Matchers::WithinAbs(-18.f, DESIRED_PRECISION));
        CHECK_THAT(result.y, Catch::Matchers::WithinAbs(1.f, DESIRED_PRECISION));
        CHECK_THAT(result.z, Catch::Matchers::WithinAbs(42.f, DESIRED_PRECISION));
    }
}
} // namespace test_matrix