#include "linalg/matrix_ops.h"

namespace linalg {
    auto lu_decomposition(const Matrix3<float>& A) -> std::pair<Matrix3<float>, Matrix3<float>> {
        // Doolittle algorithm without pivoting
        auto L = Matrix3<float>::identity();
        auto U = Matrix3<float>::zero();


        return std::make_pair(L, U);
    }

    namespace blas2 {
        auto matrix_vector_product(const Matrix3<float>& A, const Vector3<float> x) -> Vector3<float> {
            return Vector3<float>{
                A.r1c1 * x.x + A.r1c2 * x.y + A.r1c3 * x.z,
                A.r2c1 * x.x + A.r2c2 * x.y + A.r2c3 * x.z,
                A.r3c1 * x.x + A.r3c2 * x.y + A.r3c3 * x.z
            };
        }

        auto solve_lower_triangular(const Matrix3<float>& L, const Vector3<float> b) -> Vector3<float> {
            auto x1 = b.x / L.r1c1;
            auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
            auto x3 = (b.z - L.r3c1 * x1 - L.r3c2 * x2) / L.r3c3;
            return Vector3<float>{ x1, x2, x3 };
        }

        auto solve_upper_triangular(const Matrix3<float>& U, const Vector3<float> b) -> Vector3<float> {
            auto x3 = b.z / U.r3c3;
            auto x2 = (b.y - U.r2c3 * x3) / U.r2c2;
            auto x1 = (b.x - U.r1c2 * x2 - U.r1c3 * x3) / U.r1c1;
            return Vector3<float>{ x1, x2, x3 };
        }

        auto solve(const Matrix3<float>& A, const Vector3<float> b) -> Vector3<float> {
            auto [L, U] = lu_decomposition(A);
            auto y = solve_lower_triangular(L, b);
            return solve_upper_triangular(U, y);
        }
    }
}

