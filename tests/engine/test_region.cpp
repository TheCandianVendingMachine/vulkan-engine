#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/catch_test_macros.hpp>

#include <engine/pool/region.h>

using namespace::ENGINE_NS;

struct TestType {
    int a = 52;
    float b = 3.14159f;
    const char *c = "Hello, world!";
};

struct TestTypeConstructor {
    int a = 52;
    float b = 3.14159f;
    const char* c = "Hello, world!";
    TestTypeConstructor(int a, float b, const char* c): a(a), b(b), c(c) {
    }
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

TEST_CASE("Pool::Region::emplace", "[Pool][Region]") {
    SECTION("Not Initialised") {
        auto region = Region<TestType>();
        region.emplace(Index(0));
        REQUIRE(region.get(Index(0)) == nullptr);
    }
    SECTION("Initialised") {
        auto region = Region<TestType>(1);
        region.emplace(Index(0));
        REQUIRE(region.get(Index(0)) != nullptr);
        REQUIRE(region.get(Index(0))->object.a == 52);
        REQUIRE(region.get(Index(0))->object.b == 3.14159f);
        REQUIRE(strcmp(region.get(Index(0))->object.c, "Hello, world!") == 0);
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Constructor with args") {
        auto region = Region<TestTypeConstructor>(1);
        region.emplace(Index(0), -6, 5.f, "hi");
        REQUIRE(region.get(Index(0)) != nullptr);
        REQUIRE(region.get(Index(0))->object.a == -6);
        REQUIRE(region.get(Index(0))->object.b == 5.f);
        REQUIRE(strcmp(region.get(Index(0))->object.c, "hi") == 0);
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Multiple") {
        auto region = Region<TestTypeConstructor>(3);
        region.emplace(Index(0), -6, 5.f, "hi");
        region.emplace(Index(1), 0, 0.f, "");
        region.emplace(Index(2), 555, 555.f, "555");

        REQUIRE(region.get(Index(0)) != nullptr);
        REQUIRE(region.get(Index(0))->object.a == -6);
        REQUIRE(region.get(Index(0))->object.b == 5.f);
        REQUIRE(strcmp(region.get(Index(0))->object.c, "hi") == 0);

        REQUIRE(region.get(Index(1)) != nullptr);
        REQUIRE(region.get(Index(1))->object.a == 0);
        REQUIRE(region.get(Index(1))->object.b == 0.f);
        REQUIRE(strcmp(region.get(Index(1))->object.c, "") == 0);

        REQUIRE(region.get(Index(2)) != nullptr);
        REQUIRE(region.get(Index(2))->object.a == 555);
        REQUIRE(region.get(Index(2))->object.b == 555.f);
        REQUIRE(strcmp(region.get(Index(2))->object.c, "555") == 0);

        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Can't emplace overtop") {
        auto region = Region<TestType>(1);
        region.emplace(Index(0));
        REQUIRE(region.emplace(Index(0)) == nullptr);
    }
}

TEST_CASE("Pool::Region::free", "[Pool][Region]") {
    SECTION("Not Initialised") {
        auto region = Region<TestType>();
        region.free(Index(0));
        region.free(Index(1000));
    }
    SECTION("Initialised, no objects") {
        auto region = Region<TestType>(10);
        region.free(Index(0));
        REQUIRE(region.do_axioms_hold_());
        region.free(Index(1000));
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Initialised") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));

        region.get(Index(1))->object.a = 656;

        region.free(Index(0));
        REQUIRE(region.do_axioms_hold_());
        region.free(Index(1000));
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(0)) == nullptr);
        REQUIRE(region.get(Index(1)) != nullptr);
        REQUIRE(region.get(Index(1))->object.a == 656);
    }
    SECTION("Post-reserve") {
        auto region = Region<TestType>(2);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.reserve(6);
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));
        region.emplace(Index(6));

        region.free(Index(0));
        REQUIRE(region.do_axioms_hold_());
        region.free(Index(3));
        REQUIRE(region.do_axioms_hold_());
        region.free(Index(5));
        REQUIRE(region.do_axioms_hold_());
        region.free(Index(6));
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(0)) == nullptr);
        REQUIRE(region.get(Index(3)) == nullptr);
        REQUIRE(region.get(Index(5)) == nullptr);
        REQUIRE(region.get(Index(6)) == nullptr);
        REQUIRE(region.get(Index(1)) != nullptr);
    }
    SECTION("Left Neighbour") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));
        REQUIRE(region.do_axioms_hold_());

        region.free(Index(5));
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(4)) != nullptr);
        REQUIRE(region.get(Index(5)) == nullptr);
    }
    SECTION("Right Neighbour") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));

        region.free(Index(0));
        region.free(Index(1));
        region.free(Index(2));
        region.free(Index(3));
        region.free(Index(4));
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(4)) == nullptr);
        REQUIRE(region.get(Index(5)) != nullptr);
    }
    SECTION("Both Neighbours") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));


        region.free(Index(4));
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(3)) != nullptr);
        REQUIRE(region.get(Index(4)) == nullptr);
        REQUIRE(region.get(Index(5)) != nullptr);
    }
    SECTION("No Neighbours") {
        auto region = Region<TestType>(20);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));
        region.emplace(Index(6));
        REQUIRE(region.do_axioms_hold_());

        region.free(Index(2));
        region.free(Index(4));
        region.free(Index(3));
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.get(Index(3)) == nullptr);
    }
    SECTION("Beginning, right neigbour") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));


        region.free(Index(0));
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(0)) == nullptr);
        REQUIRE(region.get(Index(1)) != nullptr);
    }
    SECTION("End, left neigbour") {
        auto region = Region<TestType>(5);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));


        region.free(Index(4));
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(4)) == nullptr);
        REQUIRE(region.get(Index(3)) != nullptr);
    }
    SECTION("Beginning, no neigbour") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));


        region.free(Index(0));
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(0)) == nullptr);
    }
    SECTION("End, no neigbour") {
        auto region = Region<TestType>(4);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));


        region.free(Index(3));
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(3)) == nullptr);
    }
    SECTION("Emplace after free") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));


        region.free(Index(0));
        REQUIRE(region.do_axioms_hold_());

        region.emplace(Index(0));
        REQUIRE(region.do_axioms_hold_());

        region.free(Index(0));
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(0)) == nullptr);
    }
    SECTION("Free the end of a run") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));
        region.emplace(Index(6));
        region.emplace(Index(7));
        region.emplace(Index(8));
        region.emplace(Index(9));


        region.free(Index(9));
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Free the start of a run") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));
        region.emplace(Index(6));
        region.emplace(Index(7));
        region.emplace(Index(8));
        region.emplace(Index(9));


        region.free(Index(5));
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Free the middle of a run") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));
        region.emplace(Index(6));
        region.emplace(Index(7));
        region.emplace(Index(8));
        region.emplace(Index(9));


        region.free(Index(7));
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Free the end of a run, middle of buffer") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));
        region.emplace(Index(6));
        region.emplace(Index(7));
        region.emplace(Index(8));


        region.free(Index(8));
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Free the start of a run, middle of buffer") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));
        region.emplace(Index(6));
        region.emplace(Index(7));
        region.emplace(Index(8));


        region.free(Index(4));
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Free the middle of a run, middle of buffer") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.emplace(Index(5));
        region.emplace(Index(6));
        region.emplace(Index(7));
        region.emplace(Index(8));


        region.free(Index(6));
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Free the end of a run, start of buffer") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));


        region.free(Index(4));
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Free the start of a run, start of buffer") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));


        region.free(Index(0));
        REQUIRE(region.do_axioms_hold_());
    }
    SECTION("Free the middle of a run, start of buffer") {
        auto region = Region<TestType>(10);
        region.emplace(Index(0));
        region.emplace(Index(1));
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));


        region.free(Index(2));
        REQUIRE(region.do_axioms_hold_());
    }
}

TEST_CASE("Pool::Region::reserve", "[Pool][Region]") {
    SECTION("Not Initialised - Can reserve") {
        auto region = Region<TestType>();
        region.reserve(64);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.capacity() == 64);
    }
    SECTION("Initialised - Can extend") {
        auto region = Region<TestType>(64);
        region.reserve(70);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.capacity() == 70);
    }
    SECTION("Not Initialised - Can reserve twice") {
        auto region = Region<TestType>();
        region.reserve(70);
        region.reserve(75);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.capacity() == 75);
    }
    SECTION("Reserve less than capacity == no op") {
        auto region = Region<TestType>(1);
        region.emplace(Index(0));
        region.reserve(70);
        region.reserve(50);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.capacity() == 70);
        REQUIRE(region.get(Index(0)) != nullptr);
        REQUIRE(region.get(Index(0))->object.a == 52);
        REQUIRE(region.get(Index(0))->object.b == 3.14159f);
        REQUIRE(strcmp(region.get(Index(0))->object.c, "Hello, world!") == 0);
    }
    SECTION("Reserve with object at end") {
        auto region = Region<TestType>(1);
        region.emplace(Index(0));
        region.reserve(70);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.capacity() == 70);
        REQUIRE(region.get(Index(0)) != nullptr);
        REQUIRE(region.get(Index(0))->object.a == 52);
        REQUIRE(region.get(Index(0))->object.b == 3.14159f);
        REQUIRE(strcmp(region.get(Index(0))->object.c, "Hello, world!") == 0);
    }
    SECTION("Reserve with in-use as last multiple") {
        auto region = Region<TestType>(1);
        region.emplace(Index(0));
        region.reserve(70);
        region.emplace(Index(1));
        region.reserve(120);
        region.emplace(Index(2));
        region.emplace(Index(3));
        region.emplace(Index(4));
        region.reserve(130);
        REQUIRE(region.do_axioms_hold_());
        REQUIRE(region.capacity() == 130);

        REQUIRE(region.get(Index(0)) != nullptr);
        REQUIRE(region.get(Index(0))->object.a == 52);
        REQUIRE(region.get(Index(0))->object.b == 3.14159f);
        REQUIRE(strcmp(region.get(Index(0))->object.c, "Hello, world!") == 0);

        REQUIRE(region.get(Index(1)) != nullptr);
        REQUIRE(region.get(Index(1))->object.a == 52);
        REQUIRE(region.get(Index(1))->object.b == 3.14159f);
        REQUIRE(strcmp(region.get(Index(1))->object.c, "Hello, world!") == 0);

        REQUIRE(region.get(Index(2)) != nullptr);
        REQUIRE(region.get(Index(2))->object.a == 52);
        REQUIRE(region.get(Index(2))->object.b == 3.14159f);
        REQUIRE(strcmp(region.get(Index(2))->object.c, "Hello, world!") == 0);

        REQUIRE(region.get(Index(3)) != nullptr);
        REQUIRE(region.get(Index(3))->object.a == 52);
        REQUIRE(region.get(Index(3))->object.b == 3.14159f);
        REQUIRE(strcmp(region.get(Index(3))->object.c, "Hello, world!") == 0);

        REQUIRE(region.get(Index(4)) != nullptr);
        REQUIRE(region.get(Index(4))->object.a == 52);
        REQUIRE(region.get(Index(4))->object.b == 3.14159f);
        REQUIRE(strcmp(region.get(Index(4))->object.c, "Hello, world!") == 0);
    }
    SECTION("Doesn't clear existing objects") {
        auto region = Region<TestType>(1);
        region.emplace(Index(0));
        region.reserve(2);
        region.emplace(Index(1));
        region.reserve(20);
        REQUIRE(region.do_axioms_hold_());

        REQUIRE(region.get(Index(0)) != nullptr);
        REQUIRE(region.get(Index(0))->object.a == 52);
        REQUIRE(region.get(Index(0))->object.b == 3.14159f);
        REQUIRE(strcmp(region.get(Index(0))->object.c, "Hello, world!") == 0);

        REQUIRE(region.get(Index(1)) != nullptr);
        REQUIRE(region.get(Index(1))->object.a == 52);
        REQUIRE(region.get(Index(1))->object.b == 3.14159f);
        REQUIRE(strcmp(region.get(Index(1))->object.c, "Hello, world!") == 0);
    }
}

TEST_CASE("Pool::Region::get", "[Pool][Region]") {
}

TEST_CASE("Pool::Region::clear", "[Pool][Region]") {
}

TEST_CASE("Pool::Region::begin", "[Pool][Region]") {
}

TEST_CASE("Pool::Region::end", "[Pool][Region]") {
}
