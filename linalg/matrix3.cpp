#include "linalg/matrix_ops.h"

namespace linalg {
template <>
auto Matrix3LU<float>::from(const Matrix3<float>& A) -> Matrix3LU<float> {
    // Doolittle algorithm without pivoting
    auto LU = Matrix3<float>::identity();

    LU.r1c1 = A.r1c1;
    LU.r1c2 = A.r1c2;
    LU.r1c3 = A.r1c3;

    LU.r2c1 = A.r2c1 / LU.r1c1;
    LU.r3c1 = A.r3c1 / LU.r1c1;

    LU.r2c2 = A.r2c2 - LU.r2c1 * LU.r1c2;
    LU.r2c3 = A.r2c3 - LU.r2c1 * LU.r1c3;

    LU.r3c2 = (A.r3c2 - LU.r3c1 * LU.r1c2) / LU.r2c2;

    LU.r3c3 = A.r3c3 - LU.r3c1 * LU.r1c3 - LU.r3c2 * LU.r2c3;

    return Matrix3LU<float>{LU};
}

template <>
auto Matrix3LU<double>::from(const Matrix3<double>& A) -> Matrix3LU<double> {
    // Doolittle algorithm without pivoting
    auto LU = Matrix3<double>::identity();

    LU.r1c1 = A.r1c1;
    LU.r1c2 = A.r1c2;
    LU.r1c3 = A.r1c3;

    LU.r2c1 = A.r2c1 / LU.r1c1;
    LU.r3c1 = A.r3c1 / LU.r1c1;

    LU.r2c2 = A.r2c2 - LU.r2c1 * LU.r1c2;
    LU.r2c3 = A.r2c3 - LU.r2c1 * LU.r1c3;

    LU.r3c2 = (A.r3c2 - LU.r3c1 * LU.r1c2) / LU.r2c2;

    LU.r3c3 = A.r3c3 - LU.r3c1 * LU.r1c3 - LU.r3c2 * LU.r2c3;

    return Matrix3LU<double>{LU};
}
} // namespace linalg

namespace linalg {
namespace blas2 {
auto matrix_vector_product(const Matrix3<float>& A, const Vector3<float> x) -> Vector3<float> {
    return Vector3<float>{A.r1c1 * x.x + A.r1c2 * x.y + A.r1c3 * x.z, A.r2c1 * x.x + A.r2c2 * x.y + A.r2c3 * x.z,
                          A.r3c1 * x.x + A.r3c2 * x.y + A.r3c3 * x.z};
}

auto solve_lower_triangular(const Matrix3<float>& L, const Vector3<float> b) -> Vector3<float> {
    auto x1 = b.x / L.r1c1;
    auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
    auto x3 = (b.z - L.r3c1 * x1 - L.r3c2 * x2) / L.r3c3;
    return Vector3<float>{x1, x2, x3};
}

auto solve_upper_triangular(const Matrix3<float>& U, const Vector3<float> b) -> Vector3<float> {
    auto x3 = b.z / U.r3c3;
    auto x2 = (b.y - U.r2c3 * x3) / U.r2c2;
    auto x1 = (b.x - U.r1c2 * x2 - U.r1c3 * x3) / U.r1c1;
    return Vector3<float>{x1, x2, x3};
}

auto solve(const Matrix3<float>& A, const Vector3<float> b) -> Vector3<float> {
    auto lu = Matrix3LU<float>::from(A);
    auto y  = solve_lower_triangular(lu.lower_unit(), b);
    return solve_upper_triangular(lu.upper(), y);
}
} // namespace blas2
} // namespace linalg

namespace linalg {
namespace blas2 {
auto matrix_vector_product(const Matrix3<double>& A, const Vector3<double> x) -> Vector3<double> {
    return Vector3<double>{A.r1c1 * x.x + A.r1c2 * x.y + A.r1c3 * x.z, A.r2c1 * x.x + A.r2c2 * x.y + A.r2c3 * x.z,
                           A.r3c1 * x.x + A.r3c2 * x.y + A.r3c3 * x.z};
}

auto solve_lower_triangular(const Matrix3<double>& L, const Vector3<double> b) -> Vector3<double> {
    auto x1 = b.x / L.r1c1;
    auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
    auto x3 = (b.z - L.r3c1 * x1 - L.r3c2 * x2) / L.r3c3;
    return Vector3<double>{x1, x2, x3};
}

auto solve_upper_triangular(const Matrix3<double>& U, const Vector3<double> b) -> Vector3<double> {
    auto x3 = b.z / U.r3c3;
    auto x2 = (b.y - U.r2c3 * x3) / U.r2c2;
    auto x1 = (b.x - U.r1c2 * x2 - U.r1c3 * x3) / U.r1c1;
    return Vector3<double>{x1, x2, x3};
}

auto solve(const Matrix3<double>& A, const Vector3<double> b) -> Vector3<double> {
    auto lu = Matrix3LU<double>::from(A);
    auto y  = solve_lower_triangular(lu.lower_unit(), b);
    return solve_upper_triangular(lu.upper(), y);
}
} // namespace blas2
} // namespace linalg
