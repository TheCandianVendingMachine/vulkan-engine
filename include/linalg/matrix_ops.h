#pragma once
#include "linalg/matrix.h"
#include "linalg/vector.h"

namespace linalg {
    namespace blas2 {
        auto matrix_vector_product(const Matrix2<float>& A, const Vector2<float> x) -> Vector2<float>;
        auto solve_lower_triangular(const Matrix2<float>& L, const Vector2<float> b) -> Vector2<float>;
        auto solve_upper_triangular(const Matrix2<float>& U, const Vector2<float> b) -> Vector2<float>;
        auto solve(const Matrix2<float>& A, const Vector2<float>b) -> Vector2<float>;

        auto matrix_vector_product(const Matrix2<double>& A, const Vector2<double> x) -> Vector2<double>;
        auto solve_lower_triangular(const Matrix2<double>& L, const Vector2<double> b) -> Vector2<double>;
        auto solve_upper_triangular(const Matrix2<double>& U, const Vector2<double> b) -> Vector2<double>;
        auto solve(const Matrix2<double>& A, const Vector2<double>b) -> Vector2<double>;
    }
}

namespace linalg {
    namespace blas2 {
        auto matrix_vector_product(const Matrix3<float>& A, const Vector3<float> x) -> Vector3<float>;
        auto solve_lower_triangular(const Matrix3<float>& L, const Vector3<float> b) -> Vector3<float>;
        auto solve_upper_triangular(const Matrix3<float>& U, const Vector3<float> b) -> Vector3<float>;
        auto solve(const Matrix3<float>& A, const Vector3<float>b) -> Vector3<float>;

        auto matrix_vector_product(const Matrix3<double>& A, const Vector3<double> x) -> Vector3<double>;
        auto solve_lower_triangular(const Matrix3<double>& L, const Vector3<double> b) -> Vector3<double>;
        auto solve_upper_triangular(const Matrix3<double>& U, const Vector3<double> b) -> Vector3<double>;
        auto solve(const Matrix3<double>& A, const Vector3<double>b) -> Vector3<double>;
    }
}