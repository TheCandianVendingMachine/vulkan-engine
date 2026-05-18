#include <engine/random.h>
#include <linalg/matrix.h>
#include <linalg/matrix_ops.h>
#include <linalg/vector.h>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

namespace bench_matrix3 {
    constexpr auto range() -> engine::Range<float> {
        return {-250000.f, 250000.f};
    }

    auto random_vector(engine::Random& rng) -> linalg::Vector4<float> {
        return linalg::Vector4<float>{
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
        };
    }

    auto random_matrix(engine::Random& rng) -> linalg::Matrix4<float> {
        return linalg::Matrix4<float>{
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
        };
    }

    auto random_matrix_lower_triangular(engine::Random& rng) -> linalg::Matrix4<float> {
        return linalg::Matrix4<float>{
          rng.range(range()),
          0.f,
          0.f,
          0.f,
          rng.range(range()),
          rng.range(range()),
          0.f,
          0.f,
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          0.f,
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
        };
    }

    auto random_matrix_upper_triangular(engine::Random& rng) -> linalg::Matrix4<float> {
        return linalg::Matrix4<float>{
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          0.f,
          rng.range(range()),
          rng.range(range()),
          rng.range(range()),
          0.f,
          0.f,
          rng.range(range()),
          rng.range(range()),
          0.f,
          0.f,
          0.f,
          rng.range(range()),
        };
    }

    TEST_CASE("blas2 - bench [Matrix4<float>]", "[blas2][Matrix4][bench]") {
        auto rng = engine::Random();
        BENCHMARK_ADVANCED("linalg::lu_decomposition - random")(Catch::Benchmark::Chronometer meter) {
            auto A = random_matrix(rng);
            meter.measure([&] { return linalg::Matrix4LU<float>::from(A); });
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
} // namespace bench_matrix3