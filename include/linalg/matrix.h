#pragma once
#include "linalg/vector.h"
#include <utility>
#include <cstring>

namespace linalg {
    template<typename T>
    struct Matrix2 {
        union {
            struct {
                T r1c1; T r1c2;
                T r2c1; T r2c2;
            };
            struct {
                T c1r1; T c2r1;
                T c1r2; T c2r2;
            };
            struct {
                T a; T b;
                T c; T d;
            };
            struct {
                T m00; T m01;
                T m10; T m11;
            };
            T elements[4];
        };
        T* data = elements;

        static auto identity() -> Matrix2 {
            return Matrix2(
                T(1), T(0),
                T(0), T(1)
            );
        }

        static auto zero() -> Matrix2 {
            return Matrix2(
                T(0), T(0),
                T(0), T(0)
            );
        }

        Matrix2(const Matrix2& other) {
            *this = other;
        }
        Matrix2(Matrix2&& other) {
            *this = std::move(other);
        }
        Matrix2(T r1c1, T r1c2, T r2c1, T r2c2) :
            r1c1(r1c1), r1c2(r1c2), r2c1(r2c1), r2c2(r2c2) {
        }
        auto operator=(const Matrix2& rhs) -> Matrix2& {
            if (&rhs == this) {
                return *this;
            }
            std::memcpy(data, rhs.data, sizeof(elements));
            return *this;
        }
        auto operator=(Matrix2&& rhs) -> Matrix2& {
            if (&rhs == this) {
                return *this;
            }
            r1c1 = std::move(rhs.r1c1);
            r2c1 = std::move(rhs.r2c1);
            r1c2 = std::move(rhs.r1c2);
            r2c2 = std::move(rhs.r2c2);
            data = std::move(rhs.data);
            return *this;
        }
    };
}; 
