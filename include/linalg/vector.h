#pragma once
#include <utility>

namespace linalg {
    template<typename T>
    struct Vector2 {
        union {
            T elements[2];
            struct {
                T x;
                T y;
            };
        };
        T* data = elements;

        Vector2(T x, T y) : x(x), y(y) {}
        Vector2(const Vector2& other) {
            *this = other;
        }
        Vector2(Vector2&& other):
            x(std::move(other.x)), y(std::move(other.y)) {
        }
        auto operator=(Vector2 rhs) const -> Vector2& {
            std::memcpy(data, rhs.data, sizeof(elements));
            return *this;
        }
        auto operator=(const Vector2& rhs) const -> Vector2& {
            if (&rhs == this) {
                return *this;
            }
            std::memcpy(data, rhs.data, sizeof(elements));
            return *this;
        }
        auto operator=(Vector2&& rhs) const -> Vector2& {
            if (&rhs == this) {
                return *this;
            }
            x = std::move(rhs.x);
            y = std::move(rhs.y);
            return *this;
        }
    };

    template<typename T>
    struct Vector3 {
        union {
            T elements[3];
            struct {
                T x;
                T y;
                T z;
            };
        };
        T* data = elements;

        Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
        Vector3(const Vector3& other) {
            *this = other;
        }
        Vector3(Vector3&& other):
            x(std::move(other.x)), y(std::move(other.y)), z(std::move(other.z)) {
        }
        auto operator=(Vector3 rhs) const -> Vector3& {
            std::memcpy(data, rhs.data, sizeof(elements));
            return *this;
        }
        auto operator=(const Vector3& rhs) const -> Vector3& {
            if (&rhs == this) {
                return *this;
            }
            std::memcpy(data, rhs.data, sizeof(elements));
            return *this;
        }
        auto operator=(Vector3&& rhs) const -> Vector3& {
            if (&rhs == this) {
                return *this;
            }
            x = std::move(rhs.x);
            y = std::move(rhs.y);
            z = std::move(rhs.z);
            return *this;
        }
    };

    template<typename T>
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
        T* data = elements;

        Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
        Vector4(const Vector4& other) {
            *this = other;
        }
        Vector4(Vector4&& other):
            x(std::move(other.x)), y(std::move(other.y)), z(std::move(other.z)), data(std::move(other.data)) {
        }
        auto operator=(Vector4 rhs) const -> Vector4& {
            std::memcpy(data, rhs.data, sizeof(elements));
            return *this;
        }
        auto operator=(const Vector4& rhs) const -> Vector4& {
            if (&rhs == this) {
                return *this;
            }
            std::memcpy(data, rhs.data, sizeof(elements));
            return *this;
        }
        auto operator=(Vector4&& rhs) const -> Vector4& {
            if (&rhs == this) {
                return *this;
            }
            x = std::move(rhs.x);
            y = std::move(rhs.y);
            z = std::move(rhs.z);
            w = std::move(rhs.w);
            data = std::move(rhs.data);
            return *this;
        }
    };
}