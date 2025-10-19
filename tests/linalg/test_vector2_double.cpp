#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <vector>

#include <engine/random.h>

#include <linalg/vector.h>
#include <linalg/vector_ops.h>

#define DESIRED_PRECISION (1e-10)

constexpr auto range() -> Range<double> { return Range<double>{-250000.f, 250000.f}; }

TEST_CASE("blas1::axpy [Vector2<double>]", "[blas1][Vector2]") {
    SECTION("Single") {
        linalg::Vector2<double> x(1.0, 2.0);
        linalg::Vector2<double> y(3.0, 4.0);
        double a = 2.0;

        linalg::Vector2<double> result = linalg::blas1::axpy(a, x, y);
        CHECK_THAT(result.x, Catch::Matchers::WithinAbs(5.0, DESIRED_PRECISION));
        CHECK_THAT(result.y, Catch::Matchers::WithinAbs(8.0, DESIRED_PRECISION));
    }

    Random rng;
    SECTION("Array") {
        std::vector<double> a;
        std::vector<linalg::Vector2<double>> x;
        std::vector<linalg::Vector2<double>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
            y.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
            a.push_back(rng.range(range()));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            auto result = linalg::blas1::axpy(a[idx], x[idx], y[idx]);
            REQUIRE_THAT(result.x, Catch::Matchers::WithinAbs(a[idx] * x[idx].x + y[idx].x, DESIRED_PRECISION));
            REQUIRE_THAT(result.y, Catch::Matchers::WithinAbs(a[idx] * x[idx].y + y[idx].y, DESIRED_PRECISION));
        }
    }
}

TEST_CASE("blas1::scale [Vector2<double>]", "[blas1][Vector2]") {
    SECTION("Single") {
        linalg::Vector2<double> x(1.0, 2.0);
        double a = 2.0;

        linalg::Vector2<double> result = linalg::blas1::scale(a, x);
        CHECK_THAT(result.x, Catch::Matchers::WithinAbs(2.0, DESIRED_PRECISION));
        CHECK_THAT(result.y, Catch::Matchers::WithinAbs(4.0, DESIRED_PRECISION));
    }

    Random rng;
    SECTION("Array") {
        std::vector<double> a;
        std::vector<linalg::Vector2<double>> x;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
            a.push_back(rng.range(range()));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            auto result = linalg::blas1::scale(a[idx], x[idx]);
            REQUIRE_THAT(result.x, Catch::Matchers::WithinAbs(a[idx] * x[idx].x, DESIRED_PRECISION));
            REQUIRE_THAT(result.y, Catch::Matchers::WithinAbs(a[idx] * x[idx].y, DESIRED_PRECISION));
        }
    }
}

TEST_CASE("blas1::copy [Vector2<double>]", "[blas1][Vector2]") {
    SECTION("Single") {
        linalg::Vector2<double> x(1.0, 2.0);
        linalg::Vector2<double> y(-1.0, 2.3);

        linalg::blas1::copy(x, y);
        CHECK_THAT(x.x, Catch::Matchers::WithinAbs(y.x, DESIRED_PRECISION));
        CHECK_THAT(x.y, Catch::Matchers::WithinAbs(y.y, DESIRED_PRECISION));
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector2<double>> x;
        std::vector<linalg::Vector2<double>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
            y.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            linalg::blas1::copy(x[idx], y[idx]);
            REQUIRE_THAT(x[idx].x, Catch::Matchers::WithinAbs(y[idx].x, DESIRED_PRECISION));
            REQUIRE_THAT(x[idx].y, Catch::Matchers::WithinAbs(y[idx].y, DESIRED_PRECISION));
        }
    }
}

TEST_CASE("blas1::swap [Vector2<double>]", "[blas1][Vector2]") {
    SECTION("Single") {
        linalg::Vector2<double> x(1.0, 2.0);
        linalg::Vector2<double> y(-1.0, 2.3);

        linalg::blas1::swap(x, y);
        CHECK_THAT(x.x, Catch::Matchers::WithinAbs(-1.0, DESIRED_PRECISION));
        CHECK_THAT(x.y, Catch::Matchers::WithinAbs(2.3, DESIRED_PRECISION));
        CHECK_THAT(y.x, Catch::Matchers::WithinAbs(1.0, DESIRED_PRECISION));
        CHECK_THAT(y.y, Catch::Matchers::WithinAbs(2.0, DESIRED_PRECISION));
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector2<double>> x;
        std::vector<linalg::Vector2<double>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
            y.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            linalg::Vector2<double> x_temp = x[idx];
            linalg::Vector2<double> y_temp = y[idx];
            linalg::blas1::swap(x[idx], y[idx]);
            REQUIRE_THAT(x[idx].x, Catch::Matchers::WithinAbs(y_temp.x, DESIRED_PRECISION));
            REQUIRE_THAT(x[idx].y, Catch::Matchers::WithinAbs(y_temp.y, DESIRED_PRECISION));
            REQUIRE_THAT(y[idx].x, Catch::Matchers::WithinAbs(x_temp.x, DESIRED_PRECISION));
            REQUIRE_THAT(y[idx].y, Catch::Matchers::WithinAbs(x_temp.y, DESIRED_PRECISION));
        }
    }
}

TEST_CASE("blas1::dot [Vector2<double>]", "[blas1][Vector2]") {
    SECTION("Single") {
        linalg::Vector2<double> x(1.0, 2.0);
        linalg::Vector2<double> y(-1.0, 2.3);

        double result = linalg::blas1::dot(x, y);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(3.6, DESIRED_PRECISION));
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector2<double>> x;
        std::vector<linalg::Vector2<double>> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
            y.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            double result = linalg::blas1::dot(x[idx], y[idx]);
            REQUIRE_THAT(result, Catch::Matchers::WithinAbs(x[idx].x * y[idx].x + x[idx].y * y[idx].y, DESIRED_PRECISION));
        }
    }
}

TEST_CASE("blas1::component_sum [Vector2<double>]", "[blas1][Vector2]") {
    SECTION("Single (>0)") {
        linalg::Vector2<double> x(5.3, 1.2);

        double result = linalg::blas1::component_sum(x);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(std::abs(x.x) + std::abs(x.y), DESIRED_PRECISION));
    }

    SECTION("Single (<0)") {
        linalg::Vector2<double> x(-5.3, 1.2);

        double result = linalg::blas1::component_sum(x);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(std::abs(x.x) + std::abs(x.y), DESIRED_PRECISION));
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector2<double>> x;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            double result = linalg::blas1::component_sum(x[idx]);
            REQUIRE_THAT(result, Catch::Matchers::WithinAbs(std::abs(x[idx].x) + std::abs(x[idx].y), DESIRED_PRECISION));
        }
    }
}

TEST_CASE("blas1::magnitude [Vector2<double>]", "[blas1][Vector2]") {
    SECTION("Single (>0)") {
        linalg::Vector2<double> x(5.3, 1.2);

        double result = linalg::blas1::magnitude(x);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(std::sqrt(x.x * x.x + x.y * x.y), DESIRED_PRECISION));
    }

    SECTION("Single (<0)") {
        linalg::Vector2<double> x(-5.3, 1.2);

        double result = linalg::blas1::magnitude(x);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(std::sqrt(x.x * x.x + x.y * x.y), DESIRED_PRECISION));
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector2<double>> x;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            double result = linalg::blas1::magnitude(x[idx]);
            REQUIRE_THAT(result, Catch::Matchers::WithinAbs(std::sqrt(x[idx].x * x[idx].x + x[idx].y * x[idx].y), DESIRED_PRECISION));
        }
    }
}

TEST_CASE("blas1::component_max [Vector2<double>]", "[blas1][Vector2]") {
    SECTION("Single (>0)") {
        linalg::Vector2<double> x(1.0, 2.0);

        double result = linalg::blas1::component_max(x);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(2.0, DESIRED_PRECISION));
    }

    SECTION("Single (<0)") {
        linalg::Vector2<double> x(-12.0, 2.0);

        double result = linalg::blas1::component_max(x);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(12.0, DESIRED_PRECISION));
    }

    Random rng;
    SECTION("Array") {
        std::vector<linalg::Vector2<double>> x;
        for (int i = 0; i < 100; i++) {
            x.push_back(linalg::Vector2<double>(rng.range(range()), rng.range(range())));
        }

        for (size_t idx = 0; idx < x.size(); ++idx) {
            double result = linalg::blas1::component_max(x[idx]);
            REQUIRE_THAT(result, Catch::Matchers::WithinAbs(std::max(std::abs(x[idx].x), std::abs(x[idx].y)), DESIRED_PRECISION));
        }
    }
}
