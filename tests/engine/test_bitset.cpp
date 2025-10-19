#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <engine/bitset.h>

using namespace ::ENGINE_NS;

TEST_CASE("Bitset::Bitset", "[Bitset]") {
    SECTION("Default constructor") {
        auto bitset = Bitset();
        REQUIRE(bitset.size() == 0);
    }
    SECTION("Size constructor") {
        {
            auto bitset = Bitset(56);
            REQUIRE(bitset.size() == 56);
        }
        {
            auto bitset = Bitset(6'000);
            REQUIRE(bitset.size() == 6'000);
        }
    }
    SECTION("Copy constructor") {
        auto bitset_original = Bitset();
        bitset_original.set(0);
        bitset_original.set(2);
        bitset_original.set(4);
        bitset_original.set(8);

        Bitset bitset_copy(bitset_original);
        REQUIRE(bitset_copy[0] == 1);
        REQUIRE(bitset_copy[1] == 0);
        REQUIRE(bitset_copy[2] == 1);
        REQUIRE(bitset_copy[3] == 0);
        REQUIRE(bitset_copy[4] == 1);
        REQUIRE(bitset_copy[5] == 0);
        REQUIRE(bitset_copy[6] == 0);
        REQUIRE(bitset_copy[7] == 0);
        REQUIRE(bitset_copy[8] == 1);
        REQUIRE(bitset_original == bitset_copy);
        REQUIRE(bitset_copy.size() == bitset_original.size());
    }
    SECTION("Move constructor") {
        auto bitset_original = Bitset();
        bitset_original.set(0);
        bitset_original.set(2);
        bitset_original.set(4);
        bitset_original.set(8);

        auto bitset_copy = Bitset(std::move(bitset_original));
        REQUIRE(bitset_copy[0] == 1);
        REQUIRE(bitset_copy[1] == 0);
        REQUIRE(bitset_copy[2] == 1);
        REQUIRE(bitset_copy[3] == 0);
        REQUIRE(bitset_copy[4] == 1);
        REQUIRE(bitset_copy[5] == 0);
        REQUIRE(bitset_copy[6] == 0);
        REQUIRE(bitset_copy[7] == 0);
        REQUIRE(bitset_copy[8] == 1);
        REQUIRE(bitset_copy.size() == 9);
    }
}

TEST_CASE("Bitset::operator=", "[Bitset]") {
    SECTION("Non-move") {
        auto bitset_original = Bitset();
        bitset_original.set(0);
        bitset_original.set(2);
        bitset_original.set(4);
        bitset_original.set(8);

        auto bitset_copy = Bitset();
        bitset_copy.set(1);
        bitset_copy.set(3);
        bitset_copy.set(5);
        bitset_copy.set(7);

        bitset_copy = bitset_original;
        REQUIRE(bitset_copy[0] == 1);
        REQUIRE(bitset_copy[1] == 0);
        REQUIRE(bitset_copy[2] == 1);
        REQUIRE(bitset_copy[3] == 0);
        REQUIRE(bitset_copy[4] == 1);
        REQUIRE(bitset_copy[5] == 0);
        REQUIRE(bitset_copy[6] == 0);
        REQUIRE(bitset_copy[7] == 0);
        REQUIRE(bitset_copy[8] == 1);
        REQUIRE(bitset_original == bitset_copy);
    }
    SECTION("Move") {
        auto bitset_original = Bitset();
        bitset_original.set(0);
        bitset_original.set(2);
        bitset_original.set(4);
        bitset_original.set(8);

        auto bitset_copy = Bitset();
        bitset_copy.set(1);
        bitset_copy.set(3);
        bitset_copy.set(5);
        bitset_copy.set(7);

        bitset_copy = bitset_original;
        REQUIRE(bitset_copy[0] == 1);
        REQUIRE(bitset_copy[1] == 0);
        REQUIRE(bitset_copy[2] == 1);
        REQUIRE(bitset_copy[3] == 0);
        REQUIRE(bitset_copy[4] == 1);
        REQUIRE(bitset_copy[5] == 0);
        REQUIRE(bitset_copy[6] == 0);
        REQUIRE(bitset_copy[7] == 0);
        REQUIRE(bitset_copy[8] == 1);
    }
}

TEST_CASE("Bitset::bit_or", "[Bitset]") {
    SECTION("Method") {
        auto bitset_1 = Bitset();
        bitset_1.set(0);
        bitset_1.set(8);
        bitset_1.set(16);

        auto bitset_2 = Bitset();
        bitset_2.set(0);
        bitset_2.set(5);

        auto bitset_3 = bitset_1.bit_or(bitset_2);
        REQUIRE(bitset_3[0] == 1);
        REQUIRE(bitset_3[1] == 0);
        REQUIRE(bitset_3[2] == 0);
        REQUIRE(bitset_3[3] == 0);
        REQUIRE(bitset_3[4] == 0);
        REQUIRE(bitset_3[5] == 1);
        REQUIRE(bitset_3[6] == 0);
        REQUIRE(bitset_3[7] == 0);
        REQUIRE(bitset_3[8] == 1);
        REQUIRE(bitset_3[9] == 0);
        REQUIRE(bitset_3[10] == 0);
        REQUIRE(bitset_3[11] == 0);
        REQUIRE(bitset_3[12] == 0);
        REQUIRE(bitset_3[13] == 0);
        REQUIRE(bitset_3[14] == 0);
        REQUIRE(bitset_3[15] == 0);
        REQUIRE(bitset_3[16] == 1);
    }
    SECTION("Operator") {
        auto bitset_1 = Bitset();
        bitset_1.set(0);
        bitset_1.set(8);
        bitset_1.set(16);

        auto bitset_2 = Bitset();
        bitset_2.set(0);
        bitset_2.set(5);

        auto bitset_3 = bitset_1 | bitset_2;
        REQUIRE(bitset_3[0] == 1);
        REQUIRE(bitset_3[1] == 0);
        REQUIRE(bitset_3[2] == 0);
        REQUIRE(bitset_3[3] == 0);
        REQUIRE(bitset_3[4] == 0);
        REQUIRE(bitset_3[5] == 1);
        REQUIRE(bitset_3[6] == 0);
        REQUIRE(bitset_3[7] == 0);
        REQUIRE(bitset_3[8] == 1);
        REQUIRE(bitset_3[9] == 0);
        REQUIRE(bitset_3[10] == 0);
        REQUIRE(bitset_3[11] == 0);
        REQUIRE(bitset_3[12] == 0);
        REQUIRE(bitset_3[13] == 0);
        REQUIRE(bitset_3[14] == 0);
        REQUIRE(bitset_3[15] == 0);
        REQUIRE(bitset_3[16] == 1);
    }
    SECTION("Operator In-place") {
        auto bitset_1 = Bitset();
        bitset_1.set(0);
        bitset_1.set(8);
        bitset_1.set(16);

        auto bitset_2 = Bitset();
        bitset_2.set(0);
        bitset_2.set(5);

        bitset_2 |= bitset_1;
        REQUIRE(bitset_2[0] == 1);
        REQUIRE(bitset_2[1] == 0);
        REQUIRE(bitset_2[2] == 0);
        REQUIRE(bitset_2[3] == 0);
        REQUIRE(bitset_2[4] == 0);
        REQUIRE(bitset_2[5] == 1);
        REQUIRE(bitset_2[6] == 0);
        REQUIRE(bitset_2[7] == 0);
        REQUIRE(bitset_2[8] == 1);
        REQUIRE(bitset_2[9] == 0);
        REQUIRE(bitset_2[10] == 0);
        REQUIRE(bitset_2[11] == 0);
        REQUIRE(bitset_2[12] == 0);
        REQUIRE(bitset_2[13] == 0);
        REQUIRE(bitset_2[14] == 0);
        REQUIRE(bitset_2[15] == 0);
        REQUIRE(bitset_2[16] == 1);
    }
}

TEST_CASE("Bitset::bit_and", "[Bitset]") {
    SECTION("Method") {
        auto bitset_1 = Bitset();
        bitset_1.set(0);
        bitset_1.set(8);
        bitset_1.set(16);

        auto bitset_2 = Bitset();
        bitset_2.set(0);
        bitset_2.set(5);

        auto bitset_3 = bitset_1.bit_and(bitset_2);
        REQUIRE(bitset_3[0] == 1);
        REQUIRE(bitset_3[1] == 0);
        REQUIRE(bitset_3[2] == 0);
        REQUIRE(bitset_3[3] == 0);
        REQUIRE(bitset_3[4] == 0);
        REQUIRE(bitset_3[5] == 0);
        REQUIRE(bitset_3[6] == 0);
        REQUIRE(bitset_3[7] == 0);
        REQUIRE(bitset_3[8] == 0);
        REQUIRE(bitset_3[9] == 0);
        REQUIRE(bitset_3[10] == 0);
        REQUIRE(bitset_3[11] == 0);
        REQUIRE(bitset_3[12] == 0);
        REQUIRE(bitset_3[13] == 0);
        REQUIRE(bitset_3[14] == 0);
        REQUIRE(bitset_3[15] == 0);
        REQUIRE(bitset_3[16] == 0);
    }
    SECTION("Operator") {
        auto bitset_1 = Bitset();
        bitset_1.set(0);
        bitset_1.set(8);
        bitset_1.set(16);

        auto bitset_2 = Bitset();
        bitset_2.set(0);
        bitset_2.set(5);

        auto bitset_3 = bitset_1 & bitset_2;
        REQUIRE(bitset_3[0] == 1);
        REQUIRE(bitset_3[1] == 0);
        REQUIRE(bitset_3[2] == 0);
        REQUIRE(bitset_3[3] == 0);
        REQUIRE(bitset_3[4] == 0);
        REQUIRE(bitset_3[5] == 0);
        REQUIRE(bitset_3[6] == 0);
        REQUIRE(bitset_3[7] == 0);
        REQUIRE(bitset_3[8] == 0);
        REQUIRE(bitset_3[9] == 0);
        REQUIRE(bitset_3[10] == 0);
        REQUIRE(bitset_3[11] == 0);
        REQUIRE(bitset_3[12] == 0);
        REQUIRE(bitset_3[13] == 0);
        REQUIRE(bitset_3[14] == 0);
        REQUIRE(bitset_3[15] == 0);
        REQUIRE(bitset_3[16] == 0);
    }
    SECTION("Operator In-place") {
        auto bitset_1 = Bitset();
        bitset_1.set(0);
        bitset_1.set(8);
        bitset_1.set(16);

        auto bitset_2 = Bitset();
        bitset_2.set(0);
        bitset_2.set(5);

        bitset_2 &= bitset_1;
        REQUIRE(bitset_2[0] == 1);
        REQUIRE(bitset_2[1] == 0);
        REQUIRE(bitset_2[2] == 0);
        REQUIRE(bitset_2[3] == 0);
        REQUIRE(bitset_2[4] == 0);
        REQUIRE(bitset_2[5] == 0);
        REQUIRE(bitset_2[6] == 0);
        REQUIRE(bitset_2[7] == 0);
        REQUIRE(bitset_2[8] == 0);
        REQUIRE(bitset_2[9] == 0);
        REQUIRE(bitset_2[10] == 0);
        REQUIRE(bitset_2[11] == 0);
        REQUIRE(bitset_2[12] == 0);
        REQUIRE(bitset_2[13] == 0);
        REQUIRE(bitset_2[14] == 0);
        REQUIRE(bitset_2[15] == 0);
        REQUIRE(bitset_2[16] == 0);
    }
}

TEST_CASE("Bitset::bit_xor", "[Bitset]") {
    SECTION("Method") {
        auto bitset_1 = Bitset();
        bitset_1.set(0);
        bitset_1.set(8);
        bitset_1.set(16);

        auto bitset_2 = Bitset();
        bitset_2.set(0);
        bitset_2.set(5);

        auto bitset_3 = bitset_1.bit_xor(bitset_2);
        REQUIRE(bitset_3[0] == 0);
        REQUIRE(bitset_3[1] == 0);
        REQUIRE(bitset_3[2] == 0);
        REQUIRE(bitset_3[3] == 0);
        REQUIRE(bitset_3[4] == 0);
        REQUIRE(bitset_3[5] == 1);
        REQUIRE(bitset_3[6] == 0);
        REQUIRE(bitset_3[7] == 0);
        REQUIRE(bitset_3[8] == 1);
        REQUIRE(bitset_3[9] == 0);
        REQUIRE(bitset_3[10] == 0);
        REQUIRE(bitset_3[11] == 0);
        REQUIRE(bitset_3[12] == 0);
        REQUIRE(bitset_3[13] == 0);
        REQUIRE(bitset_3[14] == 0);
        REQUIRE(bitset_3[15] == 0);
        REQUIRE(bitset_3[16] == 1);
    }
    SECTION("Operator") {
        auto bitset_1 = Bitset();
        bitset_1.set(0);
        bitset_1.set(8);
        bitset_1.set(16);

        auto bitset_2 = Bitset();
        bitset_2.set(0);
        bitset_2.set(5);

        auto bitset_3 = bitset_1 ^ bitset_2;
        REQUIRE(bitset_3[0] == 0);
        REQUIRE(bitset_3[1] == 0);
        REQUIRE(bitset_3[2] == 0);
        REQUIRE(bitset_3[3] == 0);
        REQUIRE(bitset_3[4] == 0);
        REQUIRE(bitset_3[5] == 1);
        REQUIRE(bitset_3[6] == 0);
        REQUIRE(bitset_3[7] == 0);
        REQUIRE(bitset_3[8] == 1);
        REQUIRE(bitset_3[9] == 0);
        REQUIRE(bitset_3[10] == 0);
        REQUIRE(bitset_3[11] == 0);
        REQUIRE(bitset_3[12] == 0);
        REQUIRE(bitset_3[13] == 0);
        REQUIRE(bitset_3[14] == 0);
        REQUIRE(bitset_3[15] == 0);
        REQUIRE(bitset_3[16] == 1);
    }
    SECTION("Operator In-place") {
        auto bitset_1 = Bitset();
        bitset_1.set(0);
        bitset_1.set(8);
        bitset_1.set(16);

        auto bitset_2 = Bitset();
        bitset_2.set(0);
        bitset_2.set(5);

        bitset_2 ^= bitset_1;
        REQUIRE(bitset_2[0] == 0);
        REQUIRE(bitset_2[1] == 0);
        REQUIRE(bitset_2[2] == 0);
        REQUIRE(bitset_2[3] == 0);
        REQUIRE(bitset_2[4] == 0);
        REQUIRE(bitset_2[5] == 1);
        REQUIRE(bitset_2[6] == 0);
        REQUIRE(bitset_2[7] == 0);
        REQUIRE(bitset_2[8] == 1);
        REQUIRE(bitset_2[9] == 0);
        REQUIRE(bitset_2[10] == 0);
        REQUIRE(bitset_2[11] == 0);
        REQUIRE(bitset_2[12] == 0);
        REQUIRE(bitset_2[13] == 0);
        REQUIRE(bitset_2[14] == 0);
        REQUIRE(bitset_2[15] == 0);
        REQUIRE(bitset_2[16] == 1);
    }
}

TEST_CASE("Bitset::bit_equals", "[Bitset]") {
    SECTION("Method") {
        SECTION("Equals") {
            auto bitset_1 = Bitset();
            bitset_1.set(0);
            bitset_1.set(8);
            bitset_1.set(16);

            auto bitset_2 = Bitset();
            bitset_2.set(0);
            bitset_2.set(8);
            bitset_2.set(16);

            REQUIRE(bitset_1.bit_equals(bitset_2));
        }
        SECTION("Not-Equals") {
            auto bitset_1 = Bitset();
            bitset_1.set(0);
            bitset_1.set(8);
            bitset_1.set(16);

            auto bitset_2 = Bitset();
            bitset_2.set(0);
            bitset_2.set(8);
            bitset_2.set(5);

            REQUIRE_FALSE(bitset_1.bit_equals(bitset_2));
        }
        SECTION("Not-Equals (Size [small == large])") {
            auto bitset_1 = Bitset();
            bitset_1.set(0);
            bitset_1.set(8);
            bitset_1.set(16);

            auto bitset_2 = Bitset();
            bitset_2.set(0);
            bitset_2.set(8);
            bitset_2.set(16);
            bitset_2.set(5'000);

            REQUIRE_FALSE(bitset_1.bit_equals(bitset_2));
        }
        SECTION("Not-Equals (Size [large == small])") {
            auto bitset_1 = Bitset();
            bitset_1.set(0);
            bitset_1.set(8);
            bitset_1.set(16);
            bitset_1.set(5'000);

            auto bitset_2 = Bitset();
            bitset_2.set(0);
            bitset_2.set(8);
            bitset_2.set(16);

            REQUIRE_FALSE(bitset_1.bit_equals(bitset_2));
        }
    }
    SECTION("Operator") {
        SECTION("Equals") {
            auto bitset_1 = Bitset();
            bitset_1.set(0);
            bitset_1.set(8);
            bitset_1.set(16);

            auto bitset_2 = Bitset();
            bitset_2.set(0);
            bitset_2.set(8);
            bitset_2.set(16);

            REQUIRE(bitset_1 == bitset_2);
        }
        SECTION("Not-Equals") {
            auto bitset_1 = Bitset();
            bitset_1.set(0);
            bitset_1.set(8);
            bitset_1.set(16);

            auto bitset_2 = Bitset();
            bitset_2.set(0);
            bitset_2.set(8);
            bitset_2.set(5);

            REQUIRE_FALSE(bitset_1 == bitset_2);
        }
        SECTION("Not-Equals (Size [small == large])") {
            auto bitset_1 = Bitset();
            bitset_1.set(0);
            bitset_1.set(8);
            bitset_1.set(16);

            auto bitset_2 = Bitset();
            bitset_2.set(0);
            bitset_2.set(8);
            bitset_2.set(16);
            bitset_2.set(5'000);

            REQUIRE_FALSE(bitset_1 == bitset_2);
        }
        SECTION("Not-Equals (Size [large == small])") {
            auto bitset_1 = Bitset();
            bitset_1.set(0);
            bitset_1.set(8);
            bitset_1.set(16);
            bitset_1.set(5'000);

            auto bitset_2 = Bitset();
            bitset_2.set(0);
            bitset_2.set(8);
            bitset_2.set(16);

            REQUIRE_FALSE(bitset_1 == bitset_2);
        }
    }
}

TEST_CASE("Bitset::get", "[Bitset]") {
    SECTION("Method") {
        auto bitset = Bitset();
        bitset.set(0);
        bitset.set(1);
        bitset.set(2);
        bitset.set(200);

        REQUIRE(bitset.get(0) == 1);
        REQUIRE(bitset.get(1) == 1);
        REQUIRE(bitset.get(2) == 1);
        REQUIRE(bitset.get(3) == 0);
        REQUIRE(bitset.get(200) == 1);
        REQUIRE(bitset.get(5'000) == 0);
    }

    SECTION("Operator") {
        auto bitset = Bitset();
        bitset.set(0);
        bitset.set(1);
        bitset.set(2);
        bitset.set(200);

        REQUIRE(bitset[0] == 1);
        REQUIRE(bitset[1] == 1);
        REQUIRE(bitset[2] == 1);
        REQUIRE(bitset[3] == 0);
        REQUIRE(bitset[200] == 1);
        REQUIRE(bitset[5'000] == 0);
    }
}

TEST_CASE("Bitset::flip", "[Bitset]") {
    SECTION("in-range") {
        auto bitset = Bitset(1);

        REQUIRE(bitset[0] == 0);
        bitset.flip(0);
        REQUIRE(bitset[0] == 1);
        bitset.flip(0);
        REQUIRE(bitset[0] == 0);

        REQUIRE(bitset.size() == 1);
    }

    SECTION("out-of-range") {
        auto bitset = Bitset();

        REQUIRE(bitset[0] == 0);
        bitset.flip(0);
        REQUIRE(bitset[0] == 1);
        bitset.flip(0);
        REQUIRE(bitset[0] == 0);

        REQUIRE(bitset.size() == 1);
    }
}

TEST_CASE("Bitset::set", "[Bitset]") {
    SECTION("in-range") {
        auto bitset = Bitset(1);

        REQUIRE(bitset[0] == 0);
        bitset.set(0);
        REQUIRE(bitset[0] == 1);
        bitset.set(0);
        REQUIRE(bitset[0] == 1);

        REQUIRE(bitset.size() == 1);
    }

    SECTION("out-of-range") {
        auto bitset = Bitset();

        REQUIRE(bitset[0] == 0);
        bitset.set(0);
        REQUIRE(bitset[0] == 1);
        bitset.set(0);
        REQUIRE(bitset[0] == 1);

        REQUIRE(bitset.size() == 1);
    }

    SECTION("really really large") {
        auto bitset = Bitset();

        REQUIRE(bitset[0] == 0);
        bitset.set(0);
        REQUIRE(bitset[0] == 1);
        bitset.set(10'000);
        REQUIRE(bitset[10'000] == 1);

        REQUIRE(bitset.size() == 10'001);
    }
}

TEST_CASE("Bitset::set_to", "[Bitset]") {
    SECTION("in-range") {
        auto bitset = Bitset(1);

        REQUIRE(bitset[0] == 0);

        bitset.set_to(0, 0);
        REQUIRE(bitset[0] == 0);

        bitset.set_to(0, 1);
        REQUIRE(bitset[0] == 1);

        bitset.set_to(0, 0);
        REQUIRE(bitset[0] == 0);

        REQUIRE(bitset.size() == 1);
    }

    SECTION("out-of-range") {
        auto bitset = Bitset();

        REQUIRE(bitset[0] == 0);

        bitset.set_to(0, 0);
        REQUIRE(bitset[0] == 0);

        bitset.set_to(0, 1);
        REQUIRE(bitset[0] == 1);

        bitset.set_to(0, 0);
        REQUIRE(bitset[0] == 0);

        REQUIRE(bitset.size() == 1);
    }
}

TEST_CASE("Bitset::clear", "[Bitset]") {
    SECTION("in-range") {
        auto bitset = Bitset(1);

        REQUIRE(bitset[0] == 0);

        bitset.clear(0);
        REQUIRE(bitset[0] == 0);

        bitset.set(0);
        REQUIRE(bitset[0] == 1);

        bitset.clear(0);
        REQUIRE(bitset[0] == 0);

        REQUIRE(bitset.size() == 1);
    }

    SECTION("out-of-range") {
        auto bitset = Bitset();

        REQUIRE(bitset[0] == 0);

        bitset.clear(0);
        REQUIRE(bitset[0] == 0);

        bitset.set(0);
        REQUIRE(bitset[0] == 1);

        bitset.clear(0);
        REQUIRE(bitset[0] == 0);

        REQUIRE(bitset.size() == 1);
    }
}

TEST_CASE("Bitset::size", "[Bitset]") {
    SECTION("Empty set") {
        auto bitset = Bitset();
        REQUIRE(bitset.size() == 0);
    }

    SECTION("Sized set") {
        auto bitset = Bitset(200);
        REQUIRE(bitset.size() == 200);
    }
}

TEST_CASE("Bitset::is_subset_of", "[Bitset]") {
    auto superset = Bitset();
    superset.set(0);
    superset.set(5);
    superset.set(6);
    superset.set(9);

    SECTION("Empty set") {
        auto bitset = Bitset();
        REQUIRE(bitset.is_subset_of(superset));
    }

    SECTION("Some subset") {
        auto bitset = Bitset();
        bitset.set(0);
        bitset.set(9);
        REQUIRE(bitset.is_subset_of(superset));
    }

    SECTION("Some non-subset") {
        auto bitset = Bitset();
        bitset.set(0);
        bitset.set(9);
        bitset.set(10);
        REQUIRE_FALSE(bitset.is_subset_of(superset));
    }

    SECTION("Some bigger non-zero-set") {
        auto bitset = Bitset();
        bitset.set(0);
        bitset.set(5);
        bitset.set(6);
        bitset.set(9);
        bitset.set(10);
        REQUIRE_FALSE(bitset.is_subset_of(superset));
    }

    SECTION("Some bigger zero-set") {
        auto bitset = Bitset();
        bitset.set(0);
        bitset.set(5);
        bitset.set(6);
        bitset.set(9);
        bitset.set_to(10, 0);
        REQUIRE(bitset.is_subset_of(superset));
    }

    SECTION("Some bigger out-of-rep zero-set") {
        auto bitset = Bitset();
        bitset.set(0);
        bitset.set(5);
        bitset.set(6);
        bitset.set(9);
        bitset.set_to(5'000, 0);
        REQUIRE(bitset.is_subset_of(superset));
    }

    SECTION("Some bigger out-of-rep non-zero-set") {
        auto bitset = Bitset();
        bitset.set(0);
        bitset.set(5);
        bitset.set(6);
        bitset.set(9);
        bitset.set(5'000);
        REQUIRE_FALSE(bitset.is_subset_of(superset));
    }
}

TEST_CASE("Bitset::extend", "[Bitset]") {
    auto bitset = Bitset();
    REQUIRE(bitset.size() == 0);

    bitset.extend(50);
    REQUIRE(bitset.size() == 50);

    bitset.extend(0);
    REQUIRE(bitset.size() == 50);

    bitset.extend(100);
    REQUIRE(bitset.size() == 150);
}