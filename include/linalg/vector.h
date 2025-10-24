#pragma once
#include <cstring>
#include <utility>

namespace linalg {
    template <typename T>
    struct Vector2 {
            union {
                    T elements[2];
                    struct {
                            T x;
                            T y;
                    };
            };

            static inline constexpr auto zero() -> Vector2 {
                return Vector2(T(0), T(0));
            }

            Vector2(T x, T y) : x(x), y(y) {
            }
            Vector2(const Vector2& other) : x(other.x), y(other.y) {
            }
            Vector2(Vector2&& other) noexcept : x(std::move(other.x)), y(std::move(other.y)) {
            }
            auto operator=(const Vector2& rhs) -> Vector2& {
                if (&rhs == this) {
                    return *this;
                }
                std::memcpy(elements, rhs.elements, sizeof(elements));
                return *this;
            }
            auto operator=(Vector2&& rhs) -> Vector2& {
                if (&rhs == this) {
                    return *this;
                }
                x = std::move(rhs.x);
                y = std::move(rhs.y);
                return *this;
            }
    };

    template <typename T>
    struct Vector3 {
            union {
                    T elements[3];
                    struct {
                            T x;
                            T y;
                            T z;
                    };
            };

            static auto zero() -> Vector3 {
                return Vector3(T(0), T(0), T(0));
            }

            Vector3(T x, T y, T z) : x(x), y(y), z(z) {
            }
            Vector3(const Vector3& other) {
                *this = other;
            }
            Vector3(Vector3&& other) : x(std::move(other.x)), y(std::move(other.y)), z(std::move(other.z)) {
            }
            auto operator=(const Vector3& rhs) -> Vector3& {
                if (&rhs == this) {
                    return *this;
                }
                std::memcpy(elements, rhs.elements, sizeof(elements));
                return *this;
            }
            auto operator=(Vector3&& rhs) -> Vector3& {
                if (&rhs == this) {
                    return *this;
                }
                x = std::move(rhs.x);
                y = std::move(rhs.y);
                z = std::move(rhs.z);
                return *this;
            }
    };

    template <typename T>
    struct Vector4 {
            union {
                    T elements[4];
                    struct {
                            T x;
                            T y;
                            T z;
                            T w;
                    };
            };

            static auto zero() -> Vector4 {
                return Vector4(T(0), T(0), T(0), T(0));
            }

            Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {
            }
            Vector4(const Vector4& other) {
                *this = other;
            }
            Vector4(Vector4&& other) : x(std::move(other.x)), y(std::move(other.y)), z(std::move(other.z)), w(std::move(other.w)) {
            }
            auto operator=(const Vector4& rhs) -> Vector4& {
                if (&rhs == this) {
                    return *this;
                }
                std::memcpy(elements, rhs.elements, sizeof(elements));
                return *this;
            }
            auto operator=(Vector4&& rhs) -> Vector4& {
                if (&rhs == this) {
                    return *this;
                }
                x        = std::move(rhs.x);
                y        = std::move(rhs.y);
                z        = std::move(rhs.z);
                w        = std::move(rhs.w);
                elements = std::move(rhs.elements);
                return *this;
            }
    };
} // namespace linalg
