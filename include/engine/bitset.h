#pragma once
#include "engine/meta_defines.h"
#include <cstdint>
#include <vector>

namespace ENGINE_NS {
    class Bitset {
        public:
            using UnderlyingBitRepresentation = std::uint64_t;

            ENGINE_API Bitset()               = default;
            ENGINE_API Bitset(size_t bitcount);
            ENGINE_API Bitset(const Bitset& other)                  = default;
            ENGINE_API Bitset(Bitset&& other)                       = default;

            ENGINE_API auto operator=(const Bitset& rhs) -> Bitset& = default;
            ENGINE_API auto operator=(Bitset&& rhs) -> Bitset&      = default;

            ENGINE_API auto operator|(const Bitset& rhs) const -> Bitset;
            ENGINE_API auto operator&(const Bitset& rhs) const -> Bitset;
            ENGINE_API auto operator^(const Bitset& rhs) const -> Bitset;
            ENGINE_API auto operator==(const Bitset& rhs) const -> bool;

            ENGINE_API auto operator|=(const Bitset& rhs) -> Bitset&;
            ENGINE_API auto operator&=(const Bitset& rhs) -> Bitset&;
            ENGINE_API auto operator^=(const Bitset& rhs) -> Bitset&;

            ENGINE_API auto operator[](size_t idx) const -> std::uint8_t;

            ENGINE_API auto bit_or(const Bitset& rhs) const -> Bitset;
            ENGINE_API auto bit_and(const Bitset& rhs) const -> Bitset;
            ENGINE_API auto bit_xor(const Bitset& rhs) const -> Bitset;
            ENGINE_API auto bit_equals(const Bitset& rhs) const -> bool;

            ENGINE_API auto get(size_t idx) const -> std::uint8_t;

            ENGINE_API auto flip(size_t idx) -> void;
            ENGINE_API auto set(size_t idx) -> void;
            ENGINE_API auto set_to(size_t idx, std::uint8_t bit) -> void;
            ENGINE_API auto clear(size_t idx) -> void;

            ENGINE_API auto size() const -> size_t;
            ENGINE_API auto is_subset_of(const Bitset& superset) const -> bool;

            ENGINE_API auto extend(size_t bitcount) -> void;

            ENGINE_API auto set_bits() const -> std::vector<size_t>;

            constexpr static auto bits_to_representation_count(size_t bitcount) -> size_t {
                auto bytes = (bitcount + 8 - 1) / 8;
                return (bytes + sizeof(UnderlyingBitRepresentation) - 1) / sizeof(UnderlyingBitRepresentation);
            }

        private:
            std::vector<UnderlyingBitRepresentation> m_set;
            size_t m_bitcount   = 0;

            std::uint64_t hash_ = 0;

            auto _get_bitset_at_index(std::size_t idx) -> std::uint64_t&;
            auto _get_bitset_at_index(std::size_t idx) const -> std::uint64_t;

            template <typename T>
            friend struct std::hash;
    };
} // namespace ENGINE_NS

namespace std {
    template <class Key>
    struct hash;

    template <>
    struct hash<ENGINE_NS::Bitset> {
            auto operator()(const ENGINE_NS::Bitset& bitset) const noexcept -> std::size_t {
                return bitset.hash_;
            }
    };
} // namespace std
