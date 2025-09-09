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

#define DESIRED_PRECISION (1e-10f)

constexpr auto range() -> Range<float> {
    return Range<float>{ -15000.f, 15000.f };
}

TEST_CASE( "blas1::axpy", "[blas1][Vector2<float>]" ) {
    SECTION("Single") {
        linalg::Vector2<float> x(1.0f, 2.0f);
        linalg::Vector2<float> y(3.0f, 4.0f);
        float a = 2.0f;

        linalg::Vector2<float> result = linalg::blas1::axpy(a, x, y);
        CHECK_THAT( result.x, Catch::Matchers::WithinAbs(5.0f, DESIRED_PRECISION) );
        CHECK_THAT( result.y, Catch::Matchers::WithinAbs(8.0f, DESIRED_PRECISION) );

    }

    Random rng;
    SECTION("Array") {
        std::vector<float> a;
        std::vector<linalg::Vector2<float>> x;
        std::vector<linalg::Vector2<float>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<float>(rng.range(range()), rng.range(range())));
            y.push_back(linalg::Vector2<float>(rng.range(range()), rng.range(range())));
            a.push_back(rng.range(range()));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            auto result = linalg::blas1::axpy(a[idx], x[idx], y[idx]);
            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(a[idx] * x[idx].x + y[idx].x, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(a[idx] * x[idx].y + y[idx].y, DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::axpy - bench", "[blas1][Vector2<float>][bench]" ) {
    BENCHMARK_ADVANCED("blas1::axpy - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto a = rng.range<float>(range());
        auto x = linalg::Vector2<float>{
            rng.range(range()),
            rng.range(range())
        };
        auto y = linalg::Vector2<float>{
            rng.range(range()),
            rng.range(range())
        };
        meter.measure([&] { return linalg::blas1::axpy(a, x, y); });
    };
}


TEST_CASE( "blas1::scale", "[blas1][Vector2<float>]" ) {
    SECTION("Single") {
        linalg::Vector2<float> x(1.0f, 2.0f);
        float a = 2.0f;

        linalg::Vector2<float> result = linalg::blas1::scale(a, x);
        CHECK_THAT( result.x, Catch::Matchers::WithinAbs(2.0f, DESIRED_PRECISION) );
        CHECK_THAT( result.y, Catch::Matchers::WithinAbs(4.0f, DESIRED_PRECISION) );

    }

    Random rng;
    SECTION("Array") {
        std::vector<float> a;
        std::vector<linalg::Vector2<float>> x;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<float>(rng.range(range()), rng.range(range())));
            a.push_back(rng.range(range()));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            auto result = linalg::blas1::scale(a[idx], x[idx]);
            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(a[idx] * x[idx].x, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(a[idx] * x[idx].y, DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::scale - bench", "[blas1][Vector2<float>][bench]" ) {
    BENCHMARK_ADVANCED("blas1::scale - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto a = rng.range<float>(range());
        auto x = linalg::Vector2<float>{
            rng.range(range()),
            rng.range(range())
        };
        meter.measure([&] { return linalg::blas1::scale(a, x); });
    };
}

TEST_CASE( "blas1::copy", "[blas1][Vector2<float>]" ) {
    SECTION("Single") {
        linalg::Vector2<float> x(1.0f, 2.0f);
        linalg::Vector2<float> y(-1.0f, 2.3f);

        linalg::blas1::copy(x, y);
        CHECK_THAT( x.x, Catch::Matchers::WithinAbs(y.x, DESIRED_PRECISION) );
        CHECK_THAT( x.y, Catch::Matchers::WithinAbs(y.y, DESIRED_PRECISION) );

    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector2<float>> x;
        std::vector<linalg::Vector2<float>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<float>(rng.range(range()), rng.range(range())));
            y.push_back(linalg::Vector2<float>(rng.range(range()), rng.range(range())));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            linalg::blas1::copy(x[idx], y[idx]);
            CHECK_THAT( x[idx].x, Catch::Matchers::WithinAbs(y[idx].x, DESIRED_PRECISION) );
            CHECK_THAT( x[idx].y, Catch::Matchers::WithinAbs(y[idx].y, DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::copy - bench", "[blas1][Vector2<float>][bench]" ) {
    BENCHMARK_ADVANCED("blas1::copy - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto x = linalg::Vector2<float>{
            rng.range(range()),
            rng.range(range())
        };
        auto y = linalg::Vector2<float>{
            rng.range(range()),
            rng.range(range())
        };
        meter.measure([&] { linalg::blas1::copy(x, y); return x; });
    };
}

TEST_CASE( "blas1::swap", "[blas1][Vector2<float>]" ) {
    SECTION("Single") {
        linalg::Vector2<float> x(1.0f, 2.0f);
        linalg::Vector2<float> y(-1.0f, 2.3f);

        linalg::blas1::swap(x, y);
        CHECK_THAT( x.x, Catch::Matchers::WithinAbs(-1.0f, DESIRED_PRECISION) );
        CHECK_THAT( x.y, Catch::Matchers::WithinAbs(2.3f, DESIRED_PRECISION) );
        CHECK_THAT( y.x, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
        CHECK_THAT( y.y, Catch::Matchers::WithinAbs(2.0f, DESIRED_PRECISION) );
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector2<float>> x;
        std::vector<linalg::Vector2<float>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<float>(rng.range(range()), rng.range(range())));
            y.push_back(linalg::Vector2<float>(rng.range(range()), rng.range(range())));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            linalg::Vector2<float> x_temp = x[idx];
            linalg::Vector2<float> y_temp = y[idx];
            linalg::blas1::swap(x[idx], y[idx]);
            CHECK_THAT( x[idx].x, Catch::Matchers::WithinAbs(y_temp.x, DESIRED_PRECISION) );
            CHECK_THAT( x[idx].y, Catch::Matchers::WithinAbs(y_temp.y, DESIRED_PRECISION) );
            CHECK_THAT( y[idx].x, Catch::Matchers::WithinAbs(x_temp.x, DESIRED_PRECISION) );
            CHECK_THAT( y[idx].y, Catch::Matchers::WithinAbs(x_temp.y, DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::swap - bench", "[blas1][Vector2<float>][bench]" ) {
    BENCHMARK_ADVANCED("blas1::swap - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto x = linalg::Vector2<float>{
            rng.range(range()),
            rng.range(range())
        };
        auto y = linalg::Vector2<float>{
            rng.range(range()),
            rng.range(range())
        };
        meter.measure([&] { linalg::blas1::copy(x, y); return x; });
    };
}