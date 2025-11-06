#include "linalg/matrix_ops.h"
#ifdef __GNUG__
    #include <immintrin.h>
#else
    #include <intrin.h>
#endif

namespace linalg {
    template <>
    auto Matrix4LU<float>::from(const Matrix4<float>& A) -> Matrix4LU<float> {
        // Doolittle matrix decomposition without pivoting
        auto v_A1  = _mm_loadu_ps(&A.elements[0]);
        auto v_A2  = _mm_loadu_ps(&A.elements[4]);
        auto temp1 = _mm_shuffle_ps(v_A1, v_A2, 0b1000'1000);
        auto temp2 = _mm_shuffle_ps(v_A1, v_A2, 0b1101'1101);

        auto v_A3  = _mm_loadu_ps(&A.elements[8]);
        auto v_A4  = _mm_loadu_ps(&A.elements[12]);
        auto temp3 = _mm_shuffle_ps(v_A3, v_A4, 0b1000'1000);
        auto temp4 = _mm_shuffle_ps(v_A3, v_A4, 0b1101'1101);

        auto v_A1t = _mm_shuffle_ps(temp1, temp3, 0b1000'1000);
        auto v_A2t = _mm_shuffle_ps(temp2, temp4, 0b1000'1000);
        auto v_A3t = _mm_shuffle_ps(temp1, temp3, 0b1101'1101);
        auto v_A4t = _mm_shuffle_ps(temp2, temp4, 0b1101'1101);

        auto v_c0  = _mm_undefined_ps();
        auto v_c1  = _mm_undefined_ps();
        auto v_c2  = _mm_undefined_ps();
        auto v_c3  = _mm_undefined_ps();

        auto v_r0  = _mm_undefined_ps();
        auto v_r1  = _mm_undefined_ps();
        auto v_r2  = _mm_undefined_ps();
        auto v_r3  = _mm_undefined_ps();

        /*
        // Crout matrix decomposition without pivoting
        // Unrolled loop with commented annotations, very long
        auto LU           = Matrix4<float>::identity();
        Matrix4<float>& L = LU;
        Matrix4<float>& U = LU;

        {
            // j = 0; j < 4
            {
                // i = 0; i < 4
                {
                    // k = 0; k < 0
                    L.r0c0 = A.r0c0;
                }
                // i = 1; i < 4
                {
                    // k = 0; k < 0
                    L.r1c0 = A.r1c0;
                }
                // i = 2; i < 4
                {
                    // k = 0; k < 0
                    L.r2c0 = A.r2c0;
                }
                // i = 3; i < 4
                {
                    // k = 0; k < 0
                    L.r3c0 = A.r3c0;
                }
            }

            {
                // i = 1; i < 4
                {
                    // k = 0; k < 0
                    U.r0c1 = A.r0c1 / L.r0c0;
                }
                // i = 2; i < 4
                {
                    // k = 0; k < 0
                    U.r0c2 = A.r0c2 / L.r0c0;
                }
                // i = 3; i < 4
                {
                    // k = 0; k < 0
                    U.r0c3 = A.r0c3 / L.r0c0;
                }
            }
        }
        {
            // j = 1; j < 4
            {
                // i = 1; i < 4
                {
                    // k = 0; k < 1
                    L.r1c1 = A.r1c1 - L.r1c0 * U.r0c1;
                }
                // i = 2; i < 4
                {
                    // k = 0; k < 1
                    L.r2c1 = A.r2c1 - L.r2c0 * U.r0c1;
                }
                // i = 3; i < 4
                {
                    // k = 0; k < 1
                    L.r3c1 = A.r3c1 - L.r3c0 * U.r0c1;
                }
            }

            {
                // i = 2; i < 4
                {
                    // k = 0; k < 1
                    U.r1c2 = (A.r1c2 - L.r1c0 * U.r0c2) / L.r1c1;
                }
                // i = 3; i < 4
                {
                    // k = 0; k < 1
                    U.r1c3 = (A.r1c3 - L.r1c0 * U.r0c3) / L.r1c1;
                }
            }
        }
        {
            // j = 2; j < 4
            {
                // i = 2; i < 4
                {
                    float sum = 0.f;
                    {
                        // k = 0; k < 2
                        sum += L.r2c0 * U.r0c2;
                    }
                    {
                        // k = 1; k < 2
                        sum += L.r2c1 * U.r1c2;
                    }
                    L.r2c2 = A.r2c2 - sum;
                }
                // i = 3; i < 4
                {
                    float sum = 0.f;
                    {
                        // k = 0; k < 2
                        sum += L.r3c0 * U.r0c2;
                    }
                    {
                        // k = 1; k < 2
                        sum += L.r3c1 * U.r1c2;
                    }
                    L.r3c2 = A.r3c2 - sum;
                }
            }

            {
                // i = 3; i < 4
                {
                    float sum = 0.f;
                    {
                        // k = 0; k < 2
                        sum += L.r2c0 * U.r0c3;
                    }
                    {
                        // k = 1; k < 2
                        sum += L.r2c1 * U.r1c3;
                    }
                    U.r2c3 = (A.r2c3 - sum) / L.r2c2;
                }
            }
        }
        {
            // j = 3; j < 4
            {
                // i = 3; i < 4
                {
                    float sum = 0.f;
                    {
                        // k = 0; k < 3
                        sum += L.r3c0 * U.r0c3;
                    }
                    {
                        // k = 1; k < 3
                        sum += L.r3c1 * U.r1c3;
                    }
                    {
                        // k = 2; k < 3
                        sum += L.r3c2 * U.r2c3;
                    }
                    L.r3c3 = A.r3c3 - sum;
                }
            }
        }
        */

        float row1[4];
        float row2[4];
        float row3[4];
        float row4[4];

        Matrix4<float> LU = Matrix4<float>::zero();
        std::memcpy(LU.elements + 0, row1, 4 * sizeof(float));
        std::memcpy(LU.elements + 4, row2, 4 * sizeof(float));
        std::memcpy(LU.elements + 8, row3, 4 * sizeof(float));
        std::memcpy(LU.elements + 12, row4, 4 * sizeof(float));

        return Matrix4LU<float>{LU};
    }

    template <>
    auto Matrix4LU<double>::from(const Matrix4<double>& A) -> Matrix4LU<double> {
        // Crout matrix decomposition without pivoting
        // Unrolled loop with commented annotations, very long
        auto LU            = Matrix4<double>::identity();
        Matrix4<double>& L = LU;
        Matrix4<double>& U = LU;

        {
            // j = 0; j < 4
            {
                // i = 0; i < 4
                {
                    // k = 0; k < 0
                    L.m00 = A.m00;
                }
                // i = 1; i < 4
                {
                    // k = 0; k < 0
                    L.m10 = A.m10;
                }
                // i = 2; i < 4
                {
                    // k = 0; k < 0
                    L.m20 = A.m20;
                }
                // i = 3; i < 4
                {
                    // k = 0; k < 0
                    L.m30 = A.m30;
                }
            }

            {
                // i = 1; i < 4
                {
                    // k = 0; k < 0
                    U.m01 = A.m01 / L.m00;
                }
                // i = 2; i < 4
                {
                    // k = 0; k < 0
                    U.m02 = A.m02 / L.m00;
                }
                // i = 3; i < 4
                {
                    // k = 0; k < 0
                    U.m03 = A.m03 / L.m00;
                }
            }
        }
        {
            // j = 1; j < 4
            {
                // i = 1; i < 4
                {
                    // k = 0; k < 1
                    L.m11 = A.m11 - L.m10 * U.m01;
                }
                // i = 2; i < 4
                {
                    // k = 0; k < 1
                    L.m21 = A.m21 - L.m20 * U.m01;
                }
                // i = 3; i < 4
                {
                    // k = 0; k < 1
                    L.m31 = A.m31 - L.m30 * U.m01;
                }
            }

            {
                // i = 2; i < 4
                {
                    // k = 0; k < 1
                    U.m12 = (A.m12 - L.m10 * U.m02) / L.m11;
                }
                // i = 3; i < 4
                {
                    // k = 0; k < 1
                    U.m13 = (A.m13 - L.m10 * U.m03) / L.m11;
                }
            }
        }
        {
            // j = 2; j < 4
            {
                // i = 2; i < 4
                {
                    double sum = 0.0;
                    {
                        // k = 0; k < 2
                        sum += L.m20 * U.m02;
                    }
                    {
                        // k = 1; k < 2
                        sum += L.m21 * U.m12;
                    }
                    L.m22 = A.m22 - sum;
                }
                // i = 3; i < 4
                {
                    double sum = 0.0;
                    {
                        // k = 0; k < 2
                        sum += L.m30 * U.m02;
                    }
                    {
                        // k = 1; k < 2
                        sum += L.m31 * U.m12;
                    }
                    L.m32 = A.m32 - sum;
                }
            }

            {
                // i = 3; i < 4
                {
                    double sum = 0.0;
                    {
                        // k = 0; k < 2
                        sum += L.m20 * U.m03;
                    }
                    {
                        // k = 1; k < 2
                        sum += L.m21 * U.m13;
                    }
                    U.m23 = (A.m23 - sum) / L.m22;
                }
            }
        }
        {
            // j = 3; j < 4
            {
                // i = 3; i < 4
                {
                    double sum = L.m30 * U.m03 + L.m31 * U.m13 + L.m32 * U.m23;
                    L.m33      = A.m33 - sum;
                }
            }
        }

        return Matrix4LU<double>{LU};
    }
} // namespace linalg

namespace linalg {
    namespace blas2 {
        auto matrix_vector_product(const Matrix4<float>& A, const Vector4<float> x) -> Vector4<float> {
            auto v_x             = _mm_loadu_ps(x.elements);

            auto V_A1            = _mm_loadu_ps(&A.elements[0]);
            auto v_product1      = _mm_mul_ps(V_A1, v_x);
            auto v_product1_sum1 = _mm_add_ps(v_product1, _mm_shuffle_ps(v_product1, v_product1, 0b1011'0001));
            auto v_product1_sum2 = _mm_add_ps(v_product1_sum1, _mm_shuffle_ps(v_product1_sum1, v_product1_sum1, 0b1011'0010));

            auto V_A2            = _mm_loadu_ps(&A.elements[4]);
            auto v_product2      = _mm_mul_ps(V_A2, v_x);
            auto v_product2_sum1 = _mm_add_ps(v_product2, _mm_shuffle_ps(v_product2, v_product2, 0b1011'0001));
            auto v_product2_sum2 = _mm_add_ps(v_product2_sum1, _mm_shuffle_ps(v_product2_sum1, v_product2_sum1, 0b1011'0010));

            auto V_A3            = _mm_loadu_ps(&A.elements[8]);
            auto v_product3      = _mm_mul_ps(V_A3, v_x);
            auto v_product3_sum1 = _mm_add_ps(v_product3, _mm_shuffle_ps(v_product3, v_product3, 0b1011'0001));
            auto v_product3_sum2 = _mm_add_ps(v_product3_sum1, _mm_shuffle_ps(v_product3_sum1, v_product3_sum1, 0b1011'0010));

            auto V_A4            = _mm_loadu_ps(&A.elements[12]);
            auto v_product4      = _mm_mul_ps(V_A4, v_x);
            auto v_product4_sum1 = _mm_add_ps(v_product4, _mm_shuffle_ps(v_product4, v_product4, 0b1011'0001));
            auto v_product4_sum2 = _mm_add_ps(v_product4_sum1, _mm_shuffle_ps(v_product4_sum1, v_product4_sum1, 0b1011'0010));

            return Vector4<float>{_mm_cvtss_f32(v_product1_sum2), _mm_cvtss_f32(v_product2_sum2), _mm_cvtss_f32(v_product3_sum2),
                                  _mm_cvtss_f32(v_product4_sum2)};
        }

        auto solve_lower_triangular(const Matrix4<float>& L, const Vector4<float> b) -> Vector4<float> {
            auto x1 = b.x / L.r1c1;
            auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
            auto x3 = (b.z - L.r3c1 * x1 - L.r3c2 * x2) / L.r3c3;
            auto x4 = (b.w - L.r4c1 * x1 - L.r4c2 * x2 - L.r4c3 * x3) / L.r4c4;
            return Vector4<float>{x1, x2, x3, x4};
        }

        auto solve_upper_triangular(const Matrix4<float>& U, const Vector4<float> b) -> Vector4<float> {
            auto x4 = b.w / U.r4c4;
            auto x3 = (b.z - U.r3c4 * x4) / U.r3c3;
            auto x2 = (b.y - U.r2c3 * x3 - U.r2c4 * x4) / U.r2c2;
            auto x1 = (b.x - U.r1c2 * x2 - U.r1c3 * x3 - U.r1c4 * x4) / U.r1c1;
            return Vector4<float>{x1, x2, x3, x4};
        }

        auto solve(const Matrix4<float>& A, const Vector4<float> b) -> Vector4<float> {
            auto lu = Matrix4LU<float>::from(A);
            auto y  = solve_lower_triangular(lu.lower(), b);
            return solve_upper_triangular(lu.upper_unit(), y);
        }

        auto matrix_vector_product(const Matrix4<double>& A, const Vector4<double> x) -> Vector4<double> {
            return Vector4<double>{
              A.r1c1 * x.x + A.r1c2 * x.y + A.r1c3 * x.z + A.r1c4 * x.w, A.r2c1 * x.x + A.r2c2 * x.y + A.r2c3 * x.z + A.r2c4 * x.w,
              A.r3c1 * x.x + A.r3c2 * x.y + A.r3c3 * x.z + A.r3c4 * x.w, A.r4c1 * x.x + A.r4c2 * x.y + A.r4c3 * x.z + A.r4c4 * x.w};
        }

        auto solve_lower_triangular(const Matrix4<double>& L, const Vector4<double> b) -> Vector4<double> {
            auto x1 = b.x / L.r1c1;
            auto x2 = (b.y - L.r2c1 * x1) / L.r2c2;
            auto x3 = (b.z - L.r3c1 * x1 - L.r3c2 * x2) / L.r3c3;
            auto x4 = (b.w - L.r4c1 * x1 - L.r4c2 * x2 - L.r4c3 * x3) / L.r4c4;
            return Vector4<double>{x1, x2, x3, x4};
        }

        auto solve_upper_triangular(const Matrix4<double>& U, const Vector4<double> b) -> Vector4<double> {
            auto x4 = b.w / U.r4c4;
            auto x3 = (b.z - U.r3c4 * x4) / U.r3c3;
            auto x2 = (b.y - U.r2c3 * x3 - U.r2c4 * x4) / U.r2c2;
            auto x1 = (b.x - U.r1c2 * x2 - U.r1c3 * x3 - U.r1c4 * x4) / U.r1c1;
            return Vector4<double>{x1, x2, x3, x4};
        }

        auto solve(const Matrix4<double>& A, const Vector4<double> b) -> Vector4<double> {
            auto lu = Matrix4LU<double>::from(A);
            auto y  = solve_lower_triangular(lu.lower(), b);
            return solve_upper_triangular(lu.upper_unit(), y);
        }
    } // namespace blas2
} // namespace linalg
