#include <vector>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <engine/random.h>

#include <linalg/vector.h>
#include <linalg/vector_ops.h>

#define DESIRED_PRECISION (1e-20)

constexpr auto range() -> Range<double> {
    return Range<double>{ -250000.0, 250000.f };
}

auto random_vector(Random &rng) -> linalg::Vector4<double> {
    return linalg::Vector4<double>{
        rng.range(range()),
        rng.range(range()),
        rng.range(range()),
        rng.range(range()),
    };
}

TEST_CASE( "blas1 - bench [Vector4<double>]", "[blas1][Vector4][bench]" ) {
    BENCHMARK_ADVANCED("blas1::axpy - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto a = rng.range<double>(range());
        auto x = random_vector(rng);
        auto y = random_vector(rng);
        meter.measure([&] { return linalg::blas1::axpy(a, x, y); });
    };
    BENCHMARK_ADVANCED("blas1::scale - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto a = rng.range<double>(range());
        auto x = random_vector(rng);
        meter.measure([&] { return linalg::blas1::scale(a, x); });
    };
    BENCHMARK_ADVANCED("blas1::copy - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto x = random_vector(rng);
        auto y = random_vector(rng);
        meter.measure([&] { linalg::blas1::copy(x, y); return x; });
    };
    BENCHMARK_ADVANCED("blas1::swap - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto x = random_vector(rng);
        auto y = random_vector(rng);
        meter.measure([&] { linalg::blas1::copy(x, y); return x; });
    };
    BENCHMARK_ADVANCED("blas1::dot - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto x = random_vector(rng);
        auto y = random_vector(rng);
        meter.measure([&] { return linalg::blas1::dot(x, y); });
    };
    BENCHMARK_ADVANCED("blas1::component_sum - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto x = random_vector(rng);
        meter.measure([&] { return linalg::blas1::component_sum(x); });
    };
    BENCHMARK_ADVANCED("blas1::magnitude - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto x = random_vector(rng);
        meter.measure([&] { return linalg::blas1::magnitude(x); });
    };
    BENCHMARK_ADVANCED("blas1::component_max - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto x = random_vector(rng);
        meter.measure([&] { return linalg::blas1::component_max(x); });
    };
}