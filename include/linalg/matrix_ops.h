#pragma once
#include "linalg/matrix.h"
#include "linalg/vector.h"

namespace linalg {
    auto lu_decomposition(const Matrix2<float>& A) -> std::pair<Matrix2<float>, Matrix2<float>>;
    auto lu_decomposition(const Matrix2<double>& A) -> std::pair<Matrix2<double>, Matrix2<double>>;
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