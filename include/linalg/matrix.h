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

    template<typename T>
    struct Matrix3 {
        union {
            struct {
                T r1c1; T r1c2; T r1c3;
                T r2c1; T r2c2; T r2c3;
                T r3c1; T r3c2; T r3c3;
            };
            struct {
                T c1r1; T c2r1; T c3r1;
                T c1r2; T c2r2; T c3r2;
                T c1r3; T c2r3; T c3r3;
            };
            struct {
                T a; T b; T c;
                T d; T e; T f;
                T g; T h; T i;
            };
            struct {
                T m00; T m01; T m02;
                T m10; T m11; T m12;
                T m20; T m21; T m22;
            };
            T elements[9];
        };
        T* data = elements;

        static auto identity() -> Matrix3 {
            return Matrix3(
                T(1), T(0), T(0),
                T(0), T(1), T(0),
                T(0), T(0), T(1)
            );
        }

        static auto zero() -> Matrix3 {
            return Matrix3(
                T(0), T(0), T(0),
                T(0), T(0), T(0),
                T(0), T(0), T(0)
            );
        }

        Matrix3(const Matrix3& other) {
            *this = other;
        }
        Matrix3(Matrix3&& other) {
            *this = std::move(other);
        }
        Matrix3(T r1c1, T r1c2, T r1c3, T r2c1, T r2c2, T r2c3, T r3c1, T r3c2, T r3c3) :
            r1c1(r1c1), r1c2(r1c2), r1c3(r1c3), r2c1(r2c1), r2c2(r2c2), r2c3(r2c3), r3c1(r3c1), r3c2(r3c2), r3c3(r3c3) {
        }
        auto operator=(const Matrix3& rhs) -> Matrix3& {
            if (&rhs == this) {
                return *this;
            }
            std::memcpy(data, rhs.data, sizeof(elements));
            return *this;
        }
        auto operator=(Matrix3&& rhs) -> Matrix3& {
            if (&rhs == this) {
                return *this;
            }
            r1c1 = std::move(rhs.r1c1);
            r1c2 = std::move(rhs.r1c2);
            r1c3 = std::move(rhs.r1c3);

            r2c1 = std::move(rhs.r2c1);
            r2c2 = std::move(rhs.r2c2);
            r2c3 = std::move(rhs.r2c3);

            r3c1 = std::move(rhs.r3c1);
            r3c2 = std::move(rhs.r3c2);
            r3c3 = std::move(rhs.r3c3);
            return *this;
        }
    };
}; 

namespace linalg {
    template<typename T>
    struct Matrix2LU {
        Matrix2<T> combined;

        Matrix2LU(): combined(Matrix2<T>::zero()) {}
        Matrix2LU(const Matrix2<T>& LU): combined(LU) {}
        static auto from(const Matrix2<T>& A) -> Matrix2LU;

        auto lower() -> Matrix2<T> {
            return Matrix2<T>(
                T(1),           T(0),
                combined.r2c1,  T(1)
            );
        }
        auto upper() -> Matrix2<T> {
            return Matrix2<T>(
                combined.r1c1,  combined.r1c2,
                T(0),           combined.r2c2
            );
        }
    };

    template<typename T>
    struct Matrix3LU {
        Matrix3<T> combined;

        Matrix3LU(): combined(Matrix3<T>::zero()) {}
        Matrix3LU(const Matrix3<T>& LU): combined(LU) {}
        static auto from(const Matrix3<T>& A) -> Matrix3LU;

        auto lower() -> Matrix3<T> {
            return Matrix3<T>(
                T(1),           T(0),           T(0),
                combined.r2c1,  T(1),           T(0),
                combined.r3c1,  combined.r3c2,  T(1)
            );
        }
        auto upper() -> Matrix3<T> {
            return Matrix3<T>(
                combined.r1c1,  combined.r1c2,  combined.r1c3,
                T(0),           combined.r2c2,  combined.r2c3,
                T(0),           T(0),           combined.r3c3
            );
        }
    };
}