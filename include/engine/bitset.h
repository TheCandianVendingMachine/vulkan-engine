#pragma once
#include "engine/meta_defines.h"
#include <vector>
#include <initializer_list>
#include <cstdint>

namespace ENGINE_NS {
    class Bitset {
        public:
            using UnderlyingBitRepresentation = std::uint64_t;

            Bitset() = default;
            Bitset(size_t bitcount);
            Bitset(const Bitset &other) = default;
            Bitset(Bitset &&other) = default;
        
            auto operator=(const Bitset& rhs) -> Bitset& = default;
            auto operator=(Bitset&& rhs) -> Bitset& = default;

            auto operator|(const Bitset& rhs) const -> Bitset;
            auto operator&(const Bitset& rhs) const -> Bitset;
            auto operator^(const Bitset& rhs) const -> Bitset;
            auto operator==(const Bitset& rhs) const -> bool;

            auto operator|=(const Bitset& rhs) -> Bitset&;
            auto operator&=(const Bitset& rhs) -> Bitset&;
            auto operator^=(const Bitset& rhs) -> Bitset&;

            auto operator[](size_t idx) const -> std::uint8_t;

            auto bit_or(const Bitset& rhs) const -> Bitset;
            auto bit_and(const Bitset& rhs) const -> Bitset;
            auto bit_xor(const Bitset& rhs) const -> Bitset;
            auto bit_equals(const Bitset& rhs) const -> bool;

            auto get(size_t idx) const -> std::uint8_t;

            auto flip(size_t idx) -> void;
            auto set(size_t idx) -> void;
            auto set_to(size_t idx, std::uint8_t bit) -> void;
            auto clear(size_t idx) -> void;

            auto size() const -> size_t;
            auto is_subset_of(const Bitset& superset) const -> bool;

            auto extend(size_t bitcount) -> void;

            constexpr static auto bits_to_representation_count(size_t bitcount) -> size_t {
                auto bytes = (bitcount + 8 - 1) / 8;
                return (bytes + sizeof(UnderlyingBitRepresentation) - 1) / sizeof(UnderlyingBitRepresentation);
            }

        private:
            std::vector<UnderlyingBitRepresentation> m_set;
            size_t m_bitcount = 0;

            auto _get_bitset_at_index(std::size_t idx) -> std::uint64_t&;
            auto _get_bitset_at_index(std::size_t idx) const -> std::uint64_t;
    };
}