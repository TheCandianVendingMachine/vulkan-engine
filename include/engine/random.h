#pragma once
#include "engine/meta_defines.h"
#include <array>
#include <cstdint>
#include <limits>

namespace ENGINE_NS {
template <typename T>
struct Range {
        T lower;
        T upper;

        static constexpr auto none() -> Range {
            return {std::numeric_limits<T>::min(), std::numeric_limits<T>::max()};
        }
};

class Random {
    public:
        template <typename T>
        ENGINE_API auto range(Range<T> range = Range<T>::none()) -> T;

        ENGINE_API Random();
        ENGINE_API Random(uint64_t seed);
        ENGINE_API Random(const Random& other);
        ENGINE_API Random(Random&& other) noexcept;
        ENGINE_API auto operator=(const Random& other) -> Random&;
        ENGINE_API auto operator=(Random&& other) noexcept -> Random&;

    private:
        std::array<uint64_t, 4> m_state;
        auto random_value() -> uint64_t;
};
} // namespace ENGINE_NS
