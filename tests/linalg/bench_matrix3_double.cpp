#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <engine/random.h>

#include <linalg/vector.h>
#include <linalg/matrix.h>
#include <linalg/matrix_ops.h>

namespace bench_matrix3 {
    constexpr auto range() -> Range<double> {
        return Range<double>{ -250000.f, 250000.f };
    }

    auto random_vector(Random &rng) -> linalg::Vector3<double> {
        return linalg::Vector3<double>{
            rng.range(range()),
            rng.range(range()),
            rng.range(range()),
        };
    }

    auto random_matrix(Random &rng) -> linalg::Matrix3<double> {
        return linalg::Matrix3<double>{
            rng.range(range()), rng.range(range()), rng.range(range()),
            rng.range(range()), rng.range(range()), rng.range(range()),
            rng.range(range()), rng.range(range()), rng.range(range())
        };
    }

    auto random_matrix_lower_triangular(Random &rng) -> linalg::Matrix3<double> {
        return linalg::Matrix3<double>{
            rng.range(range()), 0.f, 0.f,
            rng.range(range()), rng.range(range()), 0.f,
            rng.range(range()), rng.range(range()), rng.range(range()),
        };
    }

    auto random_matrix_upper_triangular(Random &rng) -> linalg::Matrix3<double> {
        return linalg::Matrix3<double>{
            rng.range(range()), rng.range(range()), rng.range(range()),
            0.f,                rng.range(range()), rng.range(range()),
            0.f,                0.f,                rng.range(range()),
        };
    }

    TEST_CASE( "blas2 - bench [Matrix3<double>]", "[blas2][Matrix3][bench]" ) {
        auto rng = Random();
        BENCHMARK_ADVANCED("linalg::lu_decomposition - random")(Catch::Benchmark::Chronometer meter) {
            auto A = random_matrix(rng);
            meter.measure([&] { return linalg::Matrix3LU<double>::from(A); });
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
}