#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <engine/random.h>

#include <linalg/vector.h>
#include <linalg/vector_ops.h>

#define DESIRED_PRECISION (1e-10f)

constexpr auto range() -> Range<float> { return Range<float>{-250000.f, 250000.f}; }

auto random_vector(Random& rng) -> linalg::Vector3<float> {
    return linalg::Vector3<float>{rng.range(range()), rng.range(range()), rng.range(range())};
}

TEST_CASE("blas1 - bench [Vector3<float>]", "[blas1][Vector3][bench]") {
    auto rng = Random();
    BENCHMARK_ADVANCED("blas1::axpy - random")(Catch::Benchmark::Chronometer meter) {
        auto a = rng.range<float>(range());
        auto x = random_vector(rng);
        auto y = random_vector(rng);
        meter.measure([&] { return linalg::blas1::axpy(a, x, y); });
    };
    BENCHMARK_ADVANCED("blas1::scale - random")(Catch::Benchmark::Chronometer meter) {
        auto a = rng.range<float>(range());
        auto x = random_vector(rng);
        meter.measure([&] { return linalg::blas1::scale(a, x); });
    };
    BENCHMARK_ADVANCED("blas1::copy - random")(Catch::Benchmark::Chronometer meter) {
        auto x = random_vector(rng);
        auto y = random_vector(rng);
        meter.measure([&] {
            linalg::blas1::copy(x, y);
            return x;
        });
    };
    BENCHMARK_ADVANCED("blas1::swap - random")(Catch::Benchmark::Chronometer meter) {
        auto x = random_vector(rng);
        auto y = random_vector(rng);
        meter.measure([&] {
            linalg::blas1::copy(x, y);
            return x;
        });
    };
    BENCHMARK_ADVANCED("blas1::dot - random")(Catch::Benchmark::Chronometer meter) {
        auto x = random_vector(rng);
        auto y = random_vector(rng);
        meter.measure([&] {
            linalg::blas1::copy(x, y);
            return x;
        });
    };
    BENCHMARK_ADVANCED("blas1::component_sum - random")(Catch::Benchmark::Chronometer meter) {
        auto x = random_vector(rng);
        meter.measure([&] { return linalg::blas1::component_sum(x); });
    };
    BENCHMARK_ADVANCED("blas1::magnitude - random")(Catch::Benchmark::Chronometer meter) {
        auto x = random_vector(rng);
        meter.measure([&] { return linalg::blas1::magnitude(x); });
    };
    BENCHMARK_ADVANCED("blas1::component_max - random")(Catch::Benchmark::Chronometer meter) {
        auto x = random_vector(rng);
        meter.measure([&] { return linalg::blas1::component_max(x); });
    };
}