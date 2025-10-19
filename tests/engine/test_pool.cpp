#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <engine/pool.h>

using namespace ::ENGINE_NS;

TEST_CASE("Pool::Pool", "[Pool]") {
    SECTION("Empty") {
        auto pool = Pool<int>();

        REQUIRE(pool.size() == 0);
        REQUIRE(pool.capacity() == 0);
    }
    SECTION("Count") {
        auto pool = Pool<int>(5);

        REQUIRE(pool.size() == 0);
        REQUIRE(pool.capacity() == 5);
    }
    SECTION("Move Constructor") {
        auto pool_base = Pool<int>(5);
        auto pool      = Pool(std::move(pool_base));

        REQUIRE(pool.size() == 0);
        REQUIRE(pool.capacity() == 5);
    }
}

TEST_CASE("Pool::reserve", "[Pool]") {
    SECTION("Empty") {
        auto pool = Pool<int>();
        pool.reserve(5);

        REQUIRE(pool.size() == 0);
        REQUIRE(pool.capacity() == 5);
    }
    SECTION("With some") {
        auto pool = Pool<int>(5);
        pool.reserve(10);

        REQUIRE(pool.size() == 0);
        REQUIRE(pool.capacity() == 10);
    }
    SECTION("Less than") {
        auto pool = Pool<int>(5);
        pool.reserve(2);

        REQUIRE(pool.size() == 0);
        REQUIRE(pool.capacity() == 5);
    }
}

TEST_CASE("Pool::allocate", "[Pool]") {
    SECTION("Empty, single") {
        auto pool = Pool<int>();
        auto obj  = pool.allocate();

        REQUIRE(pool.size() == 1);
    }
    SECTION("Empty, many") {
        auto pool = Pool<int>();
        auto obj1 = pool.allocate();
        auto obj2 = pool.allocate();
        auto obj3 = pool.allocate();
        auto obj4 = pool.allocate();

        REQUIRE(pool.size() == 4);
    }
    SECTION("Empty, single, specific") {
        auto pool = Pool<int>();
        auto obj1 = pool.allocate(66);

        REQUIRE(static_cast<int>(obj1) == 66);
    }
    SECTION("Empty, many, specific") {
        auto pool = Pool<int>();
        auto obj1 = pool.allocate(66);
        auto obj2 = pool.allocate(67);
        auto obj3 = pool.allocate(68);
        auto obj4 = pool.allocate(69);

        REQUIRE(static_cast<int>(obj1) == 66);
        REQUIRE(static_cast<int>(obj2) == 67);
        REQUIRE(static_cast<int>(obj3) == 68);
        REQUIRE(static_cast<int>(obj4) == 69);
    }
    SECTION("Init, single") {
        auto pool = Pool<int>(10);
        auto obj  = pool.allocate();

        REQUIRE(pool.size() == 1);
    }
    SECTION("Init, many") {
        auto pool = Pool<int>(10);
        auto obj1 = pool.allocate();
        auto obj2 = pool.allocate();
        auto obj3 = pool.allocate();
        auto obj4 = pool.allocate();

        REQUIRE(pool.size() == 4);
    }
    SECTION("Init, single, specific") {
        auto pool = Pool<int>(10);
        auto obj1 = pool.allocate(66);

        REQUIRE(static_cast<int>(obj1) == 66);
    }
    SECTION("Init, many, specific") {
        auto pool = Pool<int>(10);
        auto obj1 = pool.allocate(66);
        auto obj2 = pool.allocate(67);
        auto obj3 = pool.allocate(68);
        auto obj4 = pool.allocate(69);

        REQUIRE(static_cast<int>(obj1) == 66);
        REQUIRE(static_cast<int>(obj2) == 67);
        REQUIRE(static_cast<int>(obj3) == 68);
        REQUIRE(static_cast<int>(obj4) == 69);
    }
}

TEST_CASE("Pool::free", "[Pool]") {
    SECTION("single") {
        auto pool = Pool<int>();
        auto obj  = pool.allocate();
        pool.free(obj);

        REQUIRE(pool.size() == 0);
        REQUIRE(obj.get() == std::nullopt);
    }
    SECTION("many") {
        auto pool = Pool<int>();
        auto obj1 = pool.allocate();
        auto obj2 = pool.allocate(5);
        pool.free(obj1);

        REQUIRE(pool.size() == 1);
        REQUIRE(obj1.get() == std::nullopt);
        REQUIRE(static_cast<int>(obj2) == 5);
    }
    SECTION("loop") {
        auto pool = Pool<int>();
        auto obj1 = pool.allocate();
        auto obj2 = pool.allocate(5);
        auto obj3 = pool.allocate(8);
        pool.free(obj2);

        auto idx = 0;
        for (auto& obj : pool) {
            switch (idx) {
                case 0:
                    REQUIRE(obj == static_cast<int>(obj1));
                    break;
                case 1:
                    REQUIRE(obj == static_cast<int>(obj3));
                    break;
            }
            idx += 1;
        }
    }
}
