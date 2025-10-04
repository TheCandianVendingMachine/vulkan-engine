#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/catch_test_macros.hpp>

#include <engine/pool/region.h>

struct TestType {
    int a = 52;
    float b = 3.14159f;
    const char *c = "Hello, world!";
};

struct alignas(32) TestTypeAligned {
    int a = -2358;
    float b = 2.71f;
    const char *c = "Aligned, world!";
};

TEST_CASE( "Pool::Region::capacity", "[Pool][Region]" ) {
    SECTION("Not Initialised") {
        auto region = Region<TestType>();
        REQUIRE(region.capacity() == 0);
    }
    SECTION("Initialised") {
        auto region = Region<TestType>(64);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.capacity() == 64);
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Extended") {
        auto region = Region<TestType>(64);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.capacity() == 64);
        REQUIRE(region.do_axioms_hold_());
        region.reserve(128);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.capacity() == 128);
    }
}

TEST_CASE( "Pool::Region::alive", "[Pool][Region]" ) {
    SECTION("Not Initialised") {
        auto region = Region<TestType>();
        REQUIRE_FALSE(region.alive());
    }
    SECTION("Initialised") {
        auto region = Region<TestType>(64);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.alive());
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Extended") {
        auto region = Region<TestType>(64);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.alive());
        REQUIRE(region.do_axioms_hold_());
        region.reserve(128);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.alive());
    }
}