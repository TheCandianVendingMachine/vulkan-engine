#include "game/input/types.h"

#include <cstddef>
#include <cstdint>
#include <functional>

namespace {
    auto hash_combine(std::size_t seed, std::size_t value) noexcept -> std::size_t {
        return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6U) + (seed >> 2U));
    }
}

auto std::hash<Input>::operator()(const Input& input) const noexcept -> std::size_t {
    auto seed = std::hash<InputId>{}(input.input_id);
    seed      = hash_combine(seed, std::hash<bool>{}(input.is_doubled));

    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&input.event);
    for (std::size_t i = 0; i < sizeof(input.event); ++i) {
        seed = hash_combine(seed, bytes[i]);
    }

    return seed;
}
