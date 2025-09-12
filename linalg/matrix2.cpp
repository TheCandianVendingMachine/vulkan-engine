#include "linalg/matrix_ops.h"

namespace linalg {
    auto lu_decomposition(const Matrix2<float>& A) -> std::pair<Matrix2<float>, Matrix2<float>> {
        // Doolittle algorithm without pivoting
        auto L = Matrix2<float>::identity();
        auto U = Matrix2<float>::zero();

        U.r1c1 = A.r1c1;
        U.r1c2 = A.r1c2;

        L.r2c1 = A.r2c1 / U.r1c1;

        U.r2c2 = A.r2c2 - L.r2c1 * U.r1c2;

        return std::make_pair(L, U);
    }

    namespace blas2 {
        auto matrix_vector_product(const Matrix2<float>& A, const Vector2<float> x) -> Vector2<float> {
            return Vector2<float>{
                A.r1c1 * x.x + A.r1c2 * x.y,
                A.r2c1 * x.x + A.r2c2 * x.y
            };
        }

        auto solve_lower_triangular(const Matrix2<float>& L, const Vector2<float> b) -> Vector2<float> {
            auto x1 = b.x / L.r1c1;
            auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
            return Vector2<float>{ x1, x2 };
        }

        auto solve_upper_triangular(const Matrix2<float>& U, const Vector2<float> b) -> Vector2<float> {
            auto x2 = b.y / U.r2c2;
            auto x1 = (b.x - U.r1c2 * x2) / U.r1c1;
            return Vector2<float>{ x1, x2 };
        }

        auto solve(const Matrix2<float>& A, const Vector2<float> b) -> Vector2<float> {
            auto [L, U] = lu_decomposition(A);
            auto y = solve_lower_triangular(L, b);
            return solve_upper_triangular(U, y);
        }
    }
}


namespace linalg {
    auto lu_decomposition(const Matrix2<double>& A) -> std::pair<Matrix2<double>, Matrix2<double>> {
        // Doolittle algorithm without pivoting
        auto L = Matrix2<double>::identity();
        auto U = Matrix2<double>::zero();

        U.r1c1 = A.r1c1;
        U.r1c2 = A.r1c2;

        L.r2c1 = A.r2c1 / U.r1c1;

        U.r2c2 = A.r2c2 - L.r2c1 * U.r1c2;

        return std::make_pair(L, U);
    }

    namespace blas2 {
        auto matrix_vector_product(const Matrix2<double>& A, const Vector2<double> x) -> Vector2<double> {
            return Vector2<double>{
                A.r1c1 * x.x + A.r1c2 * x.y,
                A.r2c1 * x.x + A.r2c2 * x.y
            };
        }

        auto solve_lower_triangular(const Matrix2<double>& L, const Vector2<double> b) -> Vector2<double> {
            auto x1 = b.x / L.r1c1;
            auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
            return Vector2<double>{ x1, x2 };
        }

        auto solve_upper_triangular(const Matrix2<double>& U, const Vector2<double> b) -> Vector2<double> {
            auto x2 = b.y / U.r2c2;
            auto x1 = (b.x - U.r1c2 * x2) / U.r1c1;
            return Vector2<double>{ x1, x2 };
        }

        auto solve(const Matrix2<double>& A, const Vector2<double> b) -> Vector2<double> {
            auto [L, U] = lu_decomposition(A);
            auto y = solve_lower_triangular(L, b);
            return solve_upper_triangular(U, y);
        }
    }
}