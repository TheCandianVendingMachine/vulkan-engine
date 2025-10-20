#include "engine/bitset.h"
#include <cassert>

using namespace ::ENGINE_NS;

Bitset::Bitset(size_t bitcount) : m_set(Bitset::bits_to_representation_count(bitcount)), m_bitcount(bitcount) {
}

auto Bitset::operator|(const Bitset& rhs) const -> Bitset {
    return this->bit_or(rhs);
}

auto Bitset::operator&(const Bitset& rhs) const -> Bitset {
    return this->bit_and(rhs);
}

auto Bitset::operator^(const Bitset& rhs) const -> Bitset {
    return this->bit_xor(rhs);
}

auto Bitset::operator==(const Bitset& rhs) const -> bool {
    return this->bit_equals(rhs);
}

auto Bitset::operator|=(const Bitset& rhs) -> Bitset& {
    if (&rhs != this) {
        *this = this->bit_or(rhs);
    }
    return *this;
}

auto Bitset::operator&=(const Bitset& rhs) -> Bitset& {
    if (&rhs != this) {
        *this = this->bit_and(rhs);
    }
    return *this;
}

auto Bitset::operator^=(const Bitset& rhs) -> Bitset& {
    if (&rhs != this) {
        *this = this->bit_xor(rhs);
    }
    return *this;
}

auto Bitset::operator[](size_t idx) const -> std::uint8_t {
    return this->get(idx);
}

auto Bitset::bit_or(const Bitset& rhs) const -> Bitset {
    auto& smaller = (this->size() <= rhs.size()) ? *this : rhs;
    auto& bigger  = (this->size() > rhs.size()) ? *this : rhs;

    auto combined = bigger;
    for (std::size_t idx = 0; idx < smaller.m_set.size(); idx++) {
        assert(combined.m_set.size() > idx);
        assert(smaller.m_set.size() > idx);
        combined.m_set[idx] |= smaller.m_set[idx];
    }
    combined.hash_ = combined.hash_ ^ ~(combined.hash_ | ~smaller.hash_);

    return combined;
}

auto Bitset::bit_and(const Bitset& rhs) const -> Bitset {
    auto& smaller = (this->size() <= rhs.size()) ? *this : rhs;
    auto& bigger  = (this->size() > rhs.size()) ? *this : rhs;

    auto combined = bigger;
    for (std::size_t idx = 0; idx < smaller.m_set.size(); idx++) {
        assert(combined.m_set.size() > idx);
        assert(smaller.m_set.size() > idx);
        combined.m_set[idx] &= smaller.m_set[idx];
    }
    for (auto idx = smaller.m_set.size(); idx < combined.m_set.size(); idx++) {
        assert(combined.m_set.size() > idx);
        combined.m_set[idx] = 0;
    }
    combined.hash_ = combined.hash_ ^ ~(combined.hash_ & ~smaller.hash_);

    return combined;
}

auto Bitset::bit_xor(const Bitset& rhs) const -> Bitset {
    auto& smaller = (this->size() <= rhs.size()) ? *this : rhs;
    auto& bigger  = (this->size() > rhs.size()) ? *this : rhs;

    auto combined = bigger;
    for (std::size_t idx = 0; idx < smaller.m_set.size(); idx++) {
        assert(combined.m_set.size() > idx);
        assert(smaller.m_set.size() > idx);
        combined.m_set[idx] ^= smaller.m_set[idx];
    }
    combined.hash_ = combined.hash_ ^ ~(combined.hash_ ^ ~smaller.hash_);

    return combined;
}

auto Bitset::bit_equals(const Bitset& rhs) const -> bool {
    if (this->size() != rhs.size()) {
        return false;
    }

    for (std::size_t idx = 0; idx < this->m_set.size(); idx++) {
        assert(this->m_set.size() > idx);
        assert(rhs.m_set.size() > idx);

        auto lhs_bits = this->m_set[idx];
        auto rhs_bits = rhs.m_set[idx];
        if (lhs_bits != rhs_bits) {
            return false;
        }
    }
    return true;
}

auto Bitset::get(size_t idx) const -> std::uint8_t {
    if (idx >= this->size()) {
        return 0;
    }
    auto set         = this->_get_bitset_at_index(idx);
    idx              = idx % (8 * sizeof(Bitset::UnderlyingBitRepresentation));

    std::uint8_t bit = static_cast<std::uint8_t>(set >> idx) & static_cast<std::uint8_t>(1);
    return bit;
}

auto Bitset::flip(size_t idx) -> void {
    if (idx >= this->size()) {
        this->extend(idx - this->size() + 1);
    }
    hash_     = hash_ ^ idx;

    auto& set = this->_get_bitset_at_index(idx);
    idx       = idx % (8 * sizeof(Bitset::UnderlyingBitRepresentation));
    hash_     = hash_ ^ (static_cast<std::uint64_t>(hash_) << idx);

    set       = set ^ (static_cast<std::uint64_t>(1) << idx);
}

auto Bitset::set(size_t idx) -> void {
    if (idx >= this->size()) {
        this->extend(idx - this->size() + 1);
    }
    hash_     = hash_ ^ idx;

    auto& set = this->_get_bitset_at_index(idx);
    idx       = idx % (8 * sizeof(Bitset::UnderlyingBitRepresentation));
    hash_     = hash_ | (static_cast<std::uint64_t>(1) << idx);

    set       = set | (static_cast<std::uint64_t>(1) << idx);
}

auto Bitset::set_to(size_t idx, std::uint8_t bit) -> void {
    if (idx >= this->size()) {
        this->extend(idx - this->size() + 1);
    }
    hash_     = hash_ ^ idx;

    auto& set = this->_get_bitset_at_index(idx);
    idx       = idx % (8 * sizeof(Bitset::UnderlyingBitRepresentation));
    hash_     = hash_ ^ (static_cast<std::uint64_t>(1) << idx);

    set       = set & ~(1 << idx);
    set       = set | (static_cast<std::uint64_t>(bit) << idx);
}

auto Bitset::clear(size_t idx) -> void {
    if (idx >= this->size()) {
        this->extend(idx - this->size() + 1);
    }
    hash_     = hash_ ^ idx;

    auto& set = this->_get_bitset_at_index(idx);
    idx       = idx % (8 * sizeof(Bitset::UnderlyingBitRepresentation));
    hash_     = hash_ & (static_cast<std::uint64_t>(1) << idx);

    set       = set & ~(static_cast<std::uint64_t>(1) << idx);
}

auto Bitset::size() const -> size_t {
    return this->m_bitcount;
}

auto Bitset::is_subset_of(const Bitset& superset) const -> bool {
    // If we are bigger than the superset, but all of the extra bits are 0, then we can say we are still a subset
    if (this->size() > superset.size()) {
        auto bits_in_chunk       = sizeof(Bitset::UnderlyingBitRepresentation) * 8;
        auto extra_bits_in_chunk = bits_in_chunk - (superset.size() % bits_in_chunk);
        for (auto idx = superset.size(); idx < extra_bits_in_chunk; idx++) {
            if (superset[idx] != 0) {
                return false;
            }
        }
        for (auto idx = superset.m_set.size(); idx < this->m_set.size(); idx++) {
            assert(this->m_set.size() > idx);
            if (this->m_set[idx] != 0) {
                return false;
            }
        }
    }

    for (std::size_t idx = 0; idx < std::min(this->m_set.size(), superset.m_set.size()); idx++) {
        assert(this->m_set.size() > idx);
        assert(superset.m_set.size() > idx);

        auto lhs_bits      = this->m_set[idx];
        auto superset_bits = superset.m_set[idx];

        if (lhs_bits != (lhs_bits & superset_bits)) {
            return false;
        }
    }
    return true;
}

auto Bitset::extend(size_t bitcount) -> void {
    auto new_size_count = Bitset::bits_to_representation_count(this->m_bitcount + bitcount);
    if (new_size_count > this->m_set.size()) {
        this->m_set.reserve(new_size_count);

        auto pushes_needed = new_size_count - this->m_set.size();
        for (std::size_t idx = 0; idx < pushes_needed; idx++) {
            this->m_set.emplace_back(0);
        }
    }
    m_bitcount += bitcount;
}

auto Bitset::_get_bitset_at_index(std::size_t idx) -> std::uint64_t& {
    auto idx_bytes = idx / 8;
    auto position  = idx_bytes / sizeof(Bitset::UnderlyingBitRepresentation);

    assert(this->m_set.size() > position);
    return this->m_set[position];
}

auto Bitset::_get_bitset_at_index(std::size_t idx) const -> std::uint64_t {
    auto idx_bytes = idx / 8;
    auto position  = idx_bytes / sizeof(Bitset::UnderlyingBitRepresentation);

    assert(this->m_set.size() > position);
    return this->m_set[position];
}
