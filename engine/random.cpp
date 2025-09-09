#include <engine/random.h>
#include <chrono>
#include <cstring>

template<> auto Random::range<int8_t>(Range<int8_t> range) -> int8_t {
    return static_cast<int8_t>(random_value() % (range.upper - range.lower + 1)) + range.lower;
}

template<> auto Random::range<int16_t>(Range<int16_t> range) -> int16_t {
    return static_cast<int16_t>(random_value() % (range.upper - range.lower + 1)) + range.lower;
}

template<> auto Random::range<int32_t>(Range<int32_t> range) -> int32_t {
    return static_cast<int32_t>(random_value() % (range.upper - range.lower + 1)) + range.lower;
}

template<> auto Random::range<int64_t>(Range<int64_t> range) -> int64_t {
    return static_cast<int64_t>(random_value() % (range.upper - range.lower + 1)) + range.lower;
}

template<> auto Random::range<uint8_t>(Range<uint8_t> range) -> uint8_t {
    return static_cast<uint8_t>(random_value() % (range.upper - range.lower + 1)) + range.lower;
}

template<> auto Random::range<uint16_t>(Range<uint16_t> range) -> uint16_t {
    return static_cast<uint16_t>(random_value() % (range.upper - range.lower + 1)) + range.lower;
}

template<> auto Random::range<uint32_t>(Range<uint32_t> range) -> uint32_t {
    return static_cast<uint32_t>(random_value() % (range.upper - range.lower + 1)) + range.lower;
}

template<> auto Random::range<uint64_t>(Range<uint64_t> range) -> uint64_t {
    return static_cast<uint64_t>(random_value() % (range.upper - range.lower + 1)) + range.lower;
}

template<> auto Random::range<float>(Range<float> range) -> float {
    auto scale = static_cast<float>(random_value()) / std::numeric_limits<float>::max();
    return scale * (range.upper - range.lower) + range.lower;
}

template<> auto Random::range<double>(Range<double> range) -> double {
    auto scale = static_cast<double>(random_value()) / std::numeric_limits<double>::max();
    return scale * (range.upper - range.lower) + range.lower;
}

uint64_t rol64(uint64_t value, unsigned int shift) {
    return (value << shift) | (value >> (64 - shift));
}

auto Random::random_value() -> uint64_t {
    uint64_t const result = rol64(m_state[0] + m_state[3], 23) + m_state[0];
    uint64_t const t = m_state[1] << 17;

    m_state[2] ^= m_state[0];
    m_state[3] ^= m_state[1];
    m_state[1] ^= m_state[2];
    m_state[0] ^= m_state[3];

    m_state[2] ^= t;
    m_state[3] = rol64(m_state[3], 45);

    return result;
}

Random::Random(): m_state({ static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()), 1, 2, 3 }) {}

Random::Random(uint64_t seed): m_state({ seed, 1, 2, 3 }) {}

Random::Random(const Random& other) {
    *this = other;
}

Random::Random(Random&& other) noexcept {
    *this = std::move(other);
}

auto Random::operator=(const Random& other) -> Random& {
    if (this != &other) {
        m_state = other.m_state;
    }
    return *this;
}

auto Random::operator=(Random&& other) noexcept -> Random& {
    if (this != &other) {
        m_state = std::move(other.m_state);
    }
    return *this;
}