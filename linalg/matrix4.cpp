#include "linalg/matrix_ops.h"

namespace linalg {
    auto Matrix4LU<float>::from(const Matrix4<float>& A) -> Matrix4LU<float> {
        // Doolittle algorithm without pivoting
        auto LU = Matrix4<float>::identity();
        Matrix4<float>& L = LU;
        Matrix4<float>& U = LU;

        // dot(L_row1, U_col1) = A.r1c1
        // dot(L_row1, U_col2) = A.r1c2
        // dot(L_row1, U_col3) = A.r1c3
        // dot(L_row1, U_col4) = A.r1c4
        U.r1c1 = A.r1c1;
        U.r1c2 = A.r1c2;
        U.r1c3 = A.r1c3;
        U.r1c4 = A.r1c4;

        // dot(L_row2, U_col1) = A.r2c1
        // dot(L_row3, U_col1) = A.r3c1
        // dot(L_row4, U_col1) = A.r4c1
        L.r2c1 = A.r2c1 / U.r1c1;
        L.r3c1 = A.r3c1 / U.r1c1;
        L.r4c1 = A.r4c1 / U.r1c1;



        // dot(L_row2, U_col2) = A.r2c2
        // dot(L_row2, U_col3) = A.r2c3
        // dot(L_row2, U_col4) = A.r2c4
        U.r2c2 = A.r2c2 - L.r2c1 * U.r1c2;
        U.r2c3 = A.r2c3 - L.r2c1 * U.r1c3;
        U.r2c4 = A.r2c4 - L.r2c1 * U.r1c4;

        // dot(L_row3, U_col2) = A.r3c2
        // dot(L_row4, U_col2) = A.r4c2
        L.r3c2 = (A.r3c2 - L.r3c1 * U.r1c2) / U.r2c2;
        L.r4c2 = (A.r4c2 - L.r4c1 * U.r1c2) / U.r2c2;



        // dot(L_row3, U_col3) = A.r3c3
        // dot(L_row3, U_col4) = A.r3c4
        U.r3c3 = A.r3c3 - L.r3c1 * U.r1c3 - L.r3c2 * U.r2c3;
        U.r3c4 = A.r3c4 - L.r3c1 * U.r1c4 - L.r3c2 * U.r2c4;

        // dot(L_row4, U_col3) = A.r4c3
        L.r4c3 = (A.r4c3 - L.r4c1 * U.r1c3 - L.r4c2 * U.r2c3) / U.r3c3;
        


        // dot(L_row4, U_col4) = A.r4c4
        U.r4c4 = A.r4c4 - L.r4c1 * U.r1c4 - L.r4c2 * U.r2c4 - L.r4c3 * U.r3c4;


        return Matrix4LU<float>{ LU };
    }

    auto Matrix4LU<double>::from(const Matrix4<double>& A) -> Matrix4LU<double> {
        // Doolittle algorithm without pivoting
        auto LU = Matrix4<double>::identity();
        Matrix4<double>& L = LU;
        Matrix4<double>& U = LU;

        // dot(L_row1, U_col1) = A.r1c1
        // dot(L_row1, U_col2) = A.r1c2
        // dot(L_row1, U_col3) = A.r1c3
        // dot(L_row1, U_col4) = A.r1c4
        U.r1c1 = A.r1c1;
        U.r1c2 = A.r1c2;
        U.r1c3 = A.r1c3;
        U.r1c4 = A.r1c4;

        // dot(L_row2, U_col1) = A.r2c1
        // dot(L_row3, U_col1) = A.r3c1
        // dot(L_row4, U_col1) = A.r4c1
        L.r2c1 = A.r2c1 / U.r1c1;
        L.r3c1 = A.r3c1 / U.r1c1;
        L.r4c1 = A.r4c1 / U.r1c1;



        // dot(L_row2, U_col2) = A.r2c2
        // dot(L_row2, U_col3) = A.r2c3
        // dot(L_row2, U_col4) = A.r2c4
        U.r2c2 = A.r2c2 - L.r2c1 * U.r1c2;
        U.r2c3 = A.r2c3 - L.r2c1 * U.r1c3;
        U.r2c4 = A.r2c4 - L.r2c1 * U.r1c4;

        // dot(L_row3, U_col2) = A.r3c2
        // dot(L_row4, U_col2) = A.r4c2
        L.r3c2 = (A.r3c2 - L.r3c1 * U.r1c2) / U.r2c2;
        L.r4c2 = (A.r4c2 - L.r4c1 * U.r1c2) / U.r2c2;



        // dot(L_row3, U_col3) = A.r3c3
        // dot(L_row3, U_col4) = A.r3c4
        U.r3c3 = A.r3c3 - L.r3c1 * U.r1c3 - L.r3c2 * U.r2c3;
        U.r3c4 = A.r3c4 - L.r3c1 * U.r1c4 - L.r3c2 * U.r2c4;

        // dot(L_row4, U_col3) = A.r4c3
        L.r4c3 = (A.r4c3 - L.r4c1 * U.r1c3 - L.r4c2 * U.r2c3) / U.r3c3;
        


        // dot(L_row4, U_col4) = A.r4c4
        U.r4c4 = A.r4c4 - L.r4c1 * U.r1c4 - L.r4c2 * U.r2c4 - L.r4c3 * U.r3c4;


        return Matrix4LU<double>{ LU };
    }
}

namespace linalg {
    namespace blas2 {
        auto matrix_vector_product(const Matrix4<float>& A, const Vector4<float> x) -> Vector4<float> {
            return Vector4<float>{
                A.r1c1 * x.x + A.r1c2 * x.y + A.r1c3 * x.z + A.r1c4 * x.w,
                A.r2c1 * x.x + A.r2c2 * x.y + A.r2c3 * x.z + A.r2c4 * x.w,
                A.r3c1 * x.x + A.r3c2 * x.y + A.r3c3 * x.z + A.r3c4 * x.w,
                A.r4c1 * x.x + A.r4c2 * x.y + A.r4c3 * x.z + A.r4c4 * x.w
            };
        }

        auto solve_lower_triangular(const Matrix4<float>& L, const Vector4<float> b) -> Vector4<float> {
            auto x1 = b.x / L.r1c1;
            auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
            auto x3 = (b.z - L.r3c1 * x1 - L.r3c2 * x2) / L.r3c3;
            auto x4 = (b.w - L.r4c1 * x1 - L.r4c2 * x2 - L.r4c3 * x3) / L.r4c4;
            return Vector4<float>{ x1, x2, x3, x4 };
        }

        auto solve_upper_triangular(const Matrix4<float>& U, const Vector4<float> b) -> Vector4<float> {
            auto x4 = b.w / U.r4c4;
            auto x3 = (b.z - U.r3c4 * x4) / U.r3c3;
            auto x2 = (b.y - U.r2c3 * x3 - U.r2c4 * x4) / U.r2c2;
            auto x1 = (b.x - U.r1c2 * x2 - U.r1c3 * x3 - U.r1c4 * x4) / U.r1c1;
            return Vector4<float>{ x1, x2, x3, x4 };
        }

        auto solve(const Matrix4<float>& A, const Vector4<float> b) -> Vector4<float> {
            auto lu = Matrix4LU<float>::from(A);
            auto y = solve_lower_triangular(lu.lower(), b);
            return solve_upper_triangular(lu.upper(), y);
        }
        

        auto matrix_vector_product(const Matrix4<double>& A, const Vector4<double> x) -> Vector4<double> {
            return Vector4<double>{
                A.r1c1 * x.x + A.r1c2 * x.y + A.r1c3 * x.z + A.r1c4 * x.w,
                A.r2c1 * x.x + A.r2c2 * x.y + A.r2c3 * x.z + A.r2c4 * x.w,
                A.r3c1 * x.x + A.r3c2 * x.y + A.r3c3 * x.z + A.r3c4 * x.w,
                A.r4c1 * x.x + A.r4c2 * x.y + A.r4c3 * x.z + A.r4c4 * x.w
            };
        }

        auto solve_lower_triangular(const Matrix4<double>& L, const Vector4<double> b) -> Vector4<double> {
            auto x1 = b.x / L.r1c1;
            auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
            auto x3 = (b.z - L.r3c1 * x1 - L.r3c2 * x2) / L.r3c3;
            auto x4 = (b.w - L.r4c1 * x1 - L.r4c2 * x2 - L.r4c3 * x3) / L.r4c4;
            return Vector4<double>{ x1, x2, x3, x4 };
        }

        auto solve_upper_triangular(const Matrix4<double>& U, const Vector4<double> b) -> Vector4<double> {
            auto x4 = b.w / U.r4c4;
            auto x3 = (b.z - U.r3c4 * x4) / U.r3c3;
            auto x2 = (b.y - U.r2c3 * x3 - U.r2c4 * x4) / U.r2c2;
            auto x1 = (b.x - U.r1c2 * x2 - U.r1c3 * x3 - U.r1c4 * x4) / U.r1c1;
            return Vector4<double>{ x1, x2, x3, x4 };
        }

        auto solve(const Matrix4<double>& A, const Vector4<double> b) -> Vector4<double> {
            auto lu = Matrix4LU<double>::from(A);
            auto y = solve_lower_triangular(lu.lower(), b);
            return solve_upper_triangular(lu.upper(), y);
        }
    }
}
