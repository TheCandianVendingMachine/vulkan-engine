#pragma once
#include "engine/meta_defines.h"
#include <array>
#include <cstdint>
#include <limits>

namespace ENGINE_NS {
    template<typename T>
    struct Range {
        T lower;
        T upper;

        static constexpr auto none() -> Range {
            return { std::numeric_limits<T>::min(), std::numeric_limits<T>::max() };
        }
    };

    class Random {
    public:
        template<typename T>
        auto range(Range<T> range = Range<T>::none()) -> T;

        Random();
        Random(uint64_t seed);
        Random(const Random& other);
        Random(Random&& other) noexcept;
        auto operator=(const Random& other)->Random&;
        auto operator=(Random&& other) noexcept -> Random&;

    private:
        std::array<uint64_t, 4> m_state;
        auto random_value() -> uint64_t;
    };
}
