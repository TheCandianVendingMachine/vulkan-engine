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
    return Range<float>{ -250000.f, 250000.f };
}

auto random_vector(Random &rng) -> linalg::Vector3<float> {
    return linalg::Vector3<float>{
        rng.range(range()),
        rng.range(range()),
        rng.range(range())
    };
}

TEST_CASE( "blas1::axpy [Vector3<float>]", "[blas1][Vector3]" ) {
    SECTION("Single") {
        linalg::Vector3<float> x(1.0f, 2.0f, 6.0f);
        linalg::Vector3<float> y(3.0f, 4.0f, 5.0f);
        float a = 2.0f;

        linalg::Vector3<float> result = linalg::blas1::axpy(a, x, y);
        CHECK_THAT( result.x, Catch::Matchers::WithinAbs(5.0f, DESIRED_PRECISION) );
        CHECK_THAT( result.y, Catch::Matchers::WithinAbs(8.0f, DESIRED_PRECISION) );
        CHECK_THAT( result.z, Catch::Matchers::WithinAbs(17.0f, DESIRED_PRECISION) );

    }

    Random rng;
    SECTION("Array") {
        std::vector<float> a;
        std::vector<linalg::Vector3<float>> x;
        std::vector<linalg::Vector3<float>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(random_vector(rng));
            y.push_back(random_vector(rng));
            a.push_back(rng.range(range()));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            auto result = linalg::blas1::axpy(a[idx], x[idx], y[idx]);
            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(a[idx] * x[idx].x + y[idx].x, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(a[idx] * x[idx].y + y[idx].y, DESIRED_PRECISION) );
            CHECK_THAT( result.z, Catch::Matchers::WithinAbs(a[idx] * x[idx].z + y[idx].z, DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::scale [Vector3<float>]", "[blas1][Vector3]" ) {
    SECTION("Single") {
        linalg::Vector3<float> x(1.0f, 2.0f, 6.0f);
        float a = 2.0f;

        linalg::Vector3<float> result = linalg::blas1::scale(a, x);
        CHECK_THAT( result.x, Catch::Matchers::WithinAbs(2.0f, DESIRED_PRECISION) );
        CHECK_THAT( result.y, Catch::Matchers::WithinAbs(4.0f, DESIRED_PRECISION) );
        CHECK_THAT( result.z, Catch::Matchers::WithinAbs(12.0f, DESIRED_PRECISION) );

    }

    Random rng;
    SECTION("Array") {
        std::vector<float> a;
        std::vector<linalg::Vector3<float>> x;
        for (int i = 0; i < 100; i++) {
            x.push_back(random_vector(rng));
            a.push_back(rng.range(range()));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            auto result = linalg::blas1::scale(a[idx], x[idx]);
            CHECK_THAT( result.x, Catch::Matchers::WithinAbs(a[idx] * x[idx].x, DESIRED_PRECISION) );
            CHECK_THAT( result.y, Catch::Matchers::WithinAbs(a[idx] * x[idx].y, DESIRED_PRECISION) );
            CHECK_THAT( result.z, Catch::Matchers::WithinAbs(a[idx] * x[idx].z, DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::copy [Vector3<float>]", "[blas1][Vector3]" ) {
    SECTION("Single") {
        linalg::Vector3<float> x(1.0f, 2.0f, 6.0f);
        linalg::Vector3<float> y(-1.0f, 2.3f, 5.0f);

        linalg::blas1::copy(x, y);
        CHECK_THAT( x.x, Catch::Matchers::WithinAbs(y.x, DESIRED_PRECISION) );
        CHECK_THAT( x.y, Catch::Matchers::WithinAbs(y.y, DESIRED_PRECISION) );
        CHECK_THAT( x.z, Catch::Matchers::WithinAbs(y.z, DESIRED_PRECISION) );

    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector3<float>> x;
        std::vector<linalg::Vector3<float>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(random_vector(rng));
            y.push_back(random_vector(rng));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            linalg::blas1::copy(x[idx], y[idx]);
            CHECK_THAT( x[idx].x, Catch::Matchers::WithinAbs(y[idx].x, DESIRED_PRECISION) );
            CHECK_THAT( x[idx].y, Catch::Matchers::WithinAbs(y[idx].y, DESIRED_PRECISION) );
            CHECK_THAT( x[idx].z, Catch::Matchers::WithinAbs(y[idx].z, DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::swap [Vector3<float>]", "[blas1][Vector3]" ) {
    SECTION("Single") {
        linalg::Vector3<float> x(1.0f, 2.0f, 6.0f);
        linalg::Vector3<float> y(-1.0f, 2.3f, 5.0f);

        linalg::blas1::swap(x, y);
        CHECK_THAT( x.x, Catch::Matchers::WithinAbs(-1.0f, DESIRED_PRECISION) );
        CHECK_THAT( x.y, Catch::Matchers::WithinAbs(2.3f, DESIRED_PRECISION) );
        CHECK_THAT( y.x, Catch::Matchers::WithinAbs(1.0f, DESIRED_PRECISION) );
        CHECK_THAT( y.y, Catch::Matchers::WithinAbs(2.0f, DESIRED_PRECISION) );
        CHECK_THAT( x.z, Catch::Matchers::WithinAbs(5.0f, DESIRED_PRECISION) );
        CHECK_THAT( y.z, Catch::Matchers::WithinAbs(6.0f, DESIRED_PRECISION) );
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector3<float>> x;
        std::vector<linalg::Vector3<float>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(random_vector(rng));
            y.push_back(random_vector(rng));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            linalg::Vector3<float> x_temp = x[idx];
            linalg::Vector3<float> y_temp = y[idx];
            linalg::blas1::swap(x[idx], y[idx]);
            CHECK_THAT( x[idx].x, Catch::Matchers::WithinAbs(y_temp.x, DESIRED_PRECISION) );
            CHECK_THAT( x[idx].y, Catch::Matchers::WithinAbs(y_temp.y, DESIRED_PRECISION) );
            CHECK_THAT( y[idx].x, Catch::Matchers::WithinAbs(x_temp.x, DESIRED_PRECISION) );
            CHECK_THAT( y[idx].y, Catch::Matchers::WithinAbs(x_temp.y, DESIRED_PRECISION) );
            CHECK_THAT( x[idx].z, Catch::Matchers::WithinAbs(y_temp.z, DESIRED_PRECISION) );
            CHECK_THAT( y[idx].z, Catch::Matchers::WithinAbs(x_temp.z, DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::dot [Vector3<float>]", "[blas1][Vector3]" ) {
    SECTION("Single") {
        linalg::Vector3<float> x(1.0f, 2.0f, 6.0f);
        linalg::Vector3<float> y(-1.0f, 2.3f, 5.0f);

        float result = linalg::blas1::dot(x, y);
        CHECK_THAT( result, Catch::Matchers::WithinAbs(33.6f, DESIRED_PRECISION) );
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector3<float>> x;
        std::vector<linalg::Vector3<float>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(random_vector(rng));
            y.push_back(random_vector(rng));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            float result = linalg::blas1::dot(x[idx], y[idx]);
            CHECK_THAT( result, Catch::Matchers::WithinAbs(x[idx].x * y[idx].x + x[idx].y * y[idx].y + x[idx].z * y[idx].z, DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::component_sum [Vector3<float>]", "[blas1][Vector3]" ) {
    SECTION("Single (>0)") {
        linalg::Vector3<float> x(5.3f, 1.2f, 6.1f);

        float result = linalg::blas1::component_sum(x);
        CHECK_THAT( result, Catch::Matchers::WithinAbs(std::abs(x.x) + std::abs(x.y) + std::abs(x.z), DESIRED_PRECISION) );
    }

    SECTION("Single (<0)") {
        linalg::Vector3<float> x(-5.3f, 1.2f, -6.1f);

        float result = linalg::blas1::component_sum(x);
        CHECK_THAT( result, Catch::Matchers::WithinAbs(std::abs(x.x) + std::abs(x.y) + std::abs(x.z), DESIRED_PRECISION) );
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector3<float>> x;
        for (int i = 0; i < 100; i++) {
            x.push_back(random_vector(rng));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            float result = linalg::blas1::component_sum(x[idx]);
            CHECK_THAT( result, Catch::Matchers::WithinAbs(std::abs(x[idx].x) + std::abs(x[idx].y) + std::abs(x[idx].z), DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::magnitude [Vector3<float>]", "[blas1][Vector3]" ) {
    SECTION("Single (>0)") {
        linalg::Vector3<float> x(5.3f, 1.2f, 6.1f);

        float result = linalg::blas1::magnitude(x);
        CHECK_THAT( result, Catch::Matchers::WithinAbs(std::sqrt(x.x * x.x + x.y * x.y + x.z * x.z), DESIRED_PRECISION) );
    }

    SECTION("Single (<0)") {
        linalg::Vector3<float> x(-5.3f, 1.2f, -6.1f);

        float result = linalg::blas1::magnitude(x);
        CHECK_THAT( result, Catch::Matchers::WithinAbs(std::sqrt(x.x * x.x + x.y * x.y + x.z * x.z), DESIRED_PRECISION) );
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector3<float>> x;
        for (int i = 0; i < 100; i++) {
            x.push_back(random_vector(rng));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            float result = linalg::blas1::magnitude(x[idx]);
            CHECK_THAT( result, Catch::Matchers::WithinAbs(std::sqrt(x[idx].x * x[idx].x + x[idx].y * x[idx].y + x[idx].z * x[idx].z), DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1::component_max [Vector3<float>]", "[blas1][Vector3]" ) {
    SECTION("Single (>0)") {
        linalg::Vector3<float> x(1.0f, 2.0f, 3.0f);

        float result = linalg::blas1::component_max(x);
        CHECK_THAT( result, Catch::Matchers::WithinAbs(3.0f, DESIRED_PRECISION) );
    }

    SECTION("Single (<0)") {
        linalg::Vector3<float> x(-12.0f, 2.0f, 3.0f);

        float result = linalg::blas1::component_max(x);
        CHECK_THAT( result, Catch::Matchers::WithinAbs(12.0f, DESIRED_PRECISION) );
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector3<float>> x;
        for (int i = 0; i < 100; i++) {
            x.push_back(random_vector(rng));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            float result = linalg::blas1::component_max(x[idx]);
            float largest1 = std::max(std::abs(x[idx].x), std::abs(x[idx].y));
            float largest2 = std::abs(x[idx].z);
            CHECK_THAT( result, Catch::Matchers::WithinAbs(std::max(largest1, largest2), DESIRED_PRECISION) );
        }
    }
}

TEST_CASE( "blas1 - bench [Vector3<float>]", "[blas1][Vector3][bench]" ) {
    BENCHMARK_ADVANCED("blas1::axpy - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto a = rng.range<float>(range());
        auto x = random_vector(rng);
        auto y = random_vector(rng);
        meter.measure([&] { return linalg::blas1::axpy(a, x, y); });
    };
    BENCHMARK_ADVANCED("blas1::scale - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto a = rng.range<float>(range());
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
    BENCHMARK_ADVANCED("blas1::dot - random")(Catch::Benchmark::Chronometer meter) {
        auto rng = Random();
        auto x = random_vector(rng);
        auto y = random_vector(rng);
        meter.measure([&] { linalg::blas1::copy(x, y); return x; });
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