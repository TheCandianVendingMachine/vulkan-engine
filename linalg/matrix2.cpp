#include "linalg/matrix_ops.h"

namespace linalg {
template <>
auto Matrix2LU<float>::from(const Matrix2<float>& A) -> Matrix2LU<float> {
    // Doolittle algorithm without pivoting
    auto LU = Matrix2<float>::identity();

    LU.r1c1 = A.r1c1;
    LU.r1c2 = A.r1c2;

    LU.r2c1 = A.r2c1 / LU.r1c1;

    LU.r2c2 = A.r2c2 - LU.r2c1 * LU.r1c2;

    return Matrix2LU<float>{LU};
}

template <>
auto Matrix2LU<double>::from(const Matrix2<double>& A) -> Matrix2LU<double> {
    // Doolittle algorithm without pivoting
    auto LU = Matrix2<double>::identity();

    LU.r1c1 = A.r1c1;
    LU.r1c2 = A.r1c2;

    LU.r2c1 = A.r2c1 / LU.r1c1;

    LU.r2c2 = A.r2c2 - LU.r2c1 * LU.r1c2;

    return Matrix2LU<double>{LU};
}
} // namespace linalg

namespace linalg {
namespace blas2 {
auto matrix_vector_product(const Matrix2<float>& A, const Vector2<float> x) -> Vector2<float> {
    return Vector2<float>{A.r1c1 * x.x + A.r1c2 * x.y, A.r2c1 * x.x + A.r2c2 * x.y};
}

auto solve_lower_triangular(const Matrix2<float>& L, const Vector2<float> b) -> Vector2<float> {
    auto x1 = b.x / L.r1c1;
    auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
    return Vector2<float>{x1, x2};
}

auto solve_upper_triangular(const Matrix2<float>& U, const Vector2<float> b) -> Vector2<float> {
    auto x2 = b.y / U.r2c2;
    auto x1 = (b.x - U.r1c2 * x2) / U.r1c1;
    return Vector2<float>{x1, x2};
}

auto solve(const Matrix2<float>& A, const Vector2<float> b) -> Vector2<float> {
    auto lu = Matrix2LU<float>::from(A);
    auto y  = solve_lower_triangular(lu.lower_unit(), b);
    return solve_upper_triangular(lu.upper(), y);
}
} // namespace blas2
} // namespace linalg

namespace linalg {
namespace blas2 {
auto matrix_vector_product(const Matrix2<double>& A, const Vector2<double> x) -> Vector2<double> {
    return Vector2<double>{A.r1c1 * x.x + A.r1c2 * x.y, A.r2c1 * x.x + A.r2c2 * x.y};
}

auto solve_lower_triangular(const Matrix2<double>& L, const Vector2<double> b) -> Vector2<double> {
    auto x1 = b.x / L.r1c1;
    auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
    return Vector2<double>{x1, x2};
}

auto solve_upper_triangular(const Matrix2<double>& U, const Vector2<double> b) -> Vector2<double> {
    auto x2 = b.y / U.r2c2;
    auto x1 = (b.x - U.r1c2 * x2) / U.r1c1;
    return Vector2<double>{x1, x2};
}

auto solve(const Matrix2<double>& A, const Vector2<double> b) -> Vector2<double> {
    auto lu = Matrix2LU<double>::from(A);
    auto y  = solve_lower_triangular(lu.lower_unit(), b);
    return solve_upper_triangular(lu.upper(), y);
}
} // namespace blas2
} // namespace linalg
