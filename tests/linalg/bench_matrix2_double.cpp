#include <engine/random.h>
#include <linalg/matrix.h>
#include <linalg/matrix_ops.h>
#include <linalg/vector.h>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

constexpr auto range() -> engine::Range<double> {
    return {.lower = -250000.0, .upper = 250000.0};
}

auto random_vector(engine::Random& rng) -> linalg::Vector2<double> {
    return linalg::Vector2<double>{
      rng.range(range()),
      rng.range(range()),
    };
}

auto random_matrix(engine::Random& rng) -> linalg::Matrix2<double> {
    return linalg::Matrix2<double>{
      rng.range(range()),
      rng.range(range()),
      rng.range(range()),
      rng.range(range()),
    };
}

auto random_matrix_lower_triangular(engine::Random& rng) -> linalg::Matrix2<double> {
    return linalg::Matrix2<double>{
      rng.range(range()),
      0.0,
      rng.range(range()),
      rng.range(range()),
    };
}

auto random_matrix_upper_triangular(engine::Random& rng) -> linalg::Matrix2<double> {
    return linalg::Matrix2<double>{
      rng.range(range()),
      rng.range(range()),
      0.0,
      rng.range(range()),
    };
}

TEST_CASE("blas2 - bench [Matrix2<double>]", "[blas2][Matrix2][bench]") {
    auto rng = engine::Random();
    BENCHMARK_ADVANCED("linalg::lu_decomposition - random")(Catch::Benchmark::Chronometer meter) {
        auto A = random_matrix(rng);
        meter.measure([&] { return linalg::Matrix2LU<double>::from(A); });
    };

    BENCHMARK_ADVANCED("linalg::blas2::matrix_vector_product - random")(Catch::Benchmark::Chronometer meter) {
        auto A = random_matrix_lower_triangular(rng);
        auto b = random_vector(rng);
        meter.measure([&] { return linalg::blas2::matrix_vector_product(A, b); });
    };

    BENCHMARK_ADVANCED("linalg::blas2::solve_lower_triangular - random")(Catch::Benchmark::Chronometer meter) {
        auto A = random_matrix_lower_triangular(rng);
        auto b = random_vector(rng);
        meter.measure([&] { return linalg::blas2::solve_lower_triangular(A, b); });
    };

    BENCHMARK_ADVANCED("linalg::blas2::solve_upper_triangular - random")(Catch::Benchmark::Chronometer meter) {
        auto A = random_matrix_upper_triangular(rng);
        auto b = random_vector(rng);
        meter.measure([&] { return linalg::blas2::solve_upper_triangular(A, b); });
    };

    BENCHMARK_ADVANCED("linalg::blas2::solve - random")(Catch::Benchmark::Chronometer meter) {
        auto A = random_matrix(rng);
        auto b = random_vector(rng);
        meter.measure([&] { return linalg::blas2::solve(A, b); });
    };
}