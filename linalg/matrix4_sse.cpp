#include "linalg/matrix_ops.h"
#ifdef __GNUG__
    #include <immintrin.h>
#else
    #include <intrin.h>
#endif

namespace linalg {
    template <>
    auto Matrix4LU<float>::from(const Matrix4<float>& A) -> Matrix4LU<float> {
        __m128 row0 = _mm_loadu_ps(&A.elements[0]);
        __m128 row1 = _mm_loadu_ps(&A.elements[4]);
        __m128 row2 = _mm_loadu_ps(&A.elements[8]);
        __m128 row3 = _mm_loadu_ps(&A.elements[12]);

        // ── j = 0: L col 0 is already A col 0; scale row 0 → U[0][1..3] ──────
        {
            float inv    = 1.0f / _mm_cvtss_f32(row0);
            __m128 scale = _mm_mul_ps(row0, _mm_set1_ps(inv));
            // _mm_move_ss(a,b): lane 0 ← b[0], lanes 1–3 ← a[1–3]
            row0         = _mm_move_ss(scale, row0); // [L00 | U01 U02 U03]
        }

        // Eliminate column 0 from rows 1–3; blend mask 0b1110 keeps lane 0 (= L[i][0])
        {
            __m128 f1 = _mm_shuffle_ps(row1, row1, 0x00); // broadcast L[1][0]
            __m128 f2 = _mm_shuffle_ps(row2, row2, 0x00);
            __m128 f3 = _mm_shuffle_ps(row3, row3, 0x00);
            row1      = _mm_blend_ps(row1, _mm_sub_ps(row1, _mm_mul_ps(f1, row0)), 0b1110);
            row2      = _mm_blend_ps(row2, _mm_sub_ps(row2, _mm_mul_ps(f2, row0)), 0b1110);
            row3      = _mm_blend_ps(row3, _mm_sub_ps(row3, _mm_mul_ps(f3, row0)), 0b1110);
        }

        // ── j = 1: scale row 1 lanes 2–3 → U[1][2..3] ────────────────────────
        {
            // _mm_shuffle_ps(x,x, 0x55): broadcast lane 1
            float inv = 1.0f / _mm_cvtss_f32(_mm_shuffle_ps(row1, row1, 0x55));
            // _mm_set_ps(e3,e2,e1,e0): [1 1 inv inv] → multiply only lanes 2–3
            row1      = _mm_mul_ps(row1, _mm_set_ps(inv, inv, 1.0f, 1.0f));
        }

        // Eliminate column 1 from rows 2–3; blend mask 0b1100 keeps lanes 0–1
        {
            __m128 f2 = _mm_shuffle_ps(row2, row2, 0x55); // broadcast L[2][1]
            __m128 f3 = _mm_shuffle_ps(row3, row3, 0x55);
            row2      = _mm_blend_ps(row2, _mm_sub_ps(row2, _mm_mul_ps(f2, row1)), 0b1100);
            row3      = _mm_blend_ps(row3, _mm_sub_ps(row3, _mm_mul_ps(f3, row1)), 0b1100);
        }

        // ── j = 2: scale row 2 lane 3 → U[2][3] ──────────────────────────────
        {
            // _mm_shuffle_ps(x,x, 0xAA): broadcast lane 2
            float inv = 1.0f / _mm_cvtss_f32(_mm_shuffle_ps(row2, row2, 0xAA));
            // _mm_set_ps(e3,e2,e1,e0): [inv 1 1 1] → multiply only lane 3
            row2      = _mm_mul_ps(row2, _mm_set_ps(inv, 1.0f, 1.0f, 1.0f));
        }

        // Eliminate column 2 from row 3; blend mask 0b1000 keeps lanes 0–2
        {
            __m128 f3 = _mm_shuffle_ps(row3, row3, 0xAA); // broadcast L[3][2]
            row3      = _mm_blend_ps(row3, _mm_sub_ps(row3, _mm_mul_ps(f3, row2)), 0b1000);
        }

        // ── j = 3: L[3][3] already sits in row3 lane 3 ───────────────────────

        Matrix4<float> LU = Matrix4<float>::zero();
        _mm_storeu_ps(&LU.elements[0], row0);
        _mm_storeu_ps(&LU.elements[4], row1);
        _mm_storeu_ps(&LU.elements[8], row2);
        _mm_storeu_ps(&LU.elements[12], row3);

        return Matrix4LU<float>{LU};
    }

    template <>
    auto Matrix4LU<double>::from(const Matrix4<double>& A) -> Matrix4LU<double> {
        // Two __m128d per row: lo = [col0, col1], hi = [col2, col3]
        __m128d r0l = _mm_loadu_pd(&A.elements[0]);
        __m128d r0h = _mm_loadu_pd(&A.elements[2]);
        __m128d r1l = _mm_loadu_pd(&A.elements[4]);
        __m128d r1h = _mm_loadu_pd(&A.elements[6]);
        __m128d r2l = _mm_loadu_pd(&A.elements[8]);
        __m128d r2h = _mm_loadu_pd(&A.elements[10]);
        __m128d r3l = _mm_loadu_pd(&A.elements[12]);
        __m128d r3h = _mm_loadu_pd(&A.elements[14]);

        // ── j=0: pivot = r0l[0]; scale lanes 1–3 → U[0][1..3] ───────────────
        {
            double inv     = 1.0 / _mm_cvtsd_f64(r0l);
            __m128d vscale = _mm_set1_pd(inv);
            r0l            = _mm_blend_pd(r0l, _mm_mul_pd(r0l, vscale), 0b10); // keep L[0][0], scale U[0][1]
            r0h            = _mm_mul_pd(r0h, vscale);                          // U[0][2], U[0][3]
        }

        // Eliminate col 0 from rows 1–3
        // lo: keep lane 0 (L[i][0]), update lane 1;  hi: update both
        {
            __m128d f1 = _mm_movedup_pd(r1l); // broadcast L[1][0]
            __m128d f2 = _mm_movedup_pd(r2l);
            __m128d f3 = _mm_movedup_pd(r3l);
            r1l        = _mm_blend_pd(r1l, _mm_sub_pd(r1l, _mm_mul_pd(f1, r0l)), 0b10);
            r2l        = _mm_blend_pd(r2l, _mm_sub_pd(r2l, _mm_mul_pd(f2, r0l)), 0b10);
            r3l        = _mm_blend_pd(r3l, _mm_sub_pd(r3l, _mm_mul_pd(f3, r0l)), 0b10);
            r1h        = _mm_sub_pd(r1h, _mm_mul_pd(f1, r0h));
            r2h        = _mm_sub_pd(r2h, _mm_mul_pd(f2, r0h));
            r3h        = _mm_sub_pd(r3h, _mm_mul_pd(f3, r0h));
        }
        // After this step: r_il = [L[i][0], L[i][1]] — both lo lanes are final L entries

        // ── j=1: pivot = r1l[1]; scale r1h → U[1][2..3] ─────────────────────
        // r1l is all L — no blend needed, just scale hi
        {
            double inv     = 1.0 / _mm_cvtsd_f64(_mm_unpackhi_pd(r1l, r1l)); // extract lane 1
            __m128d vscale = _mm_set1_pd(inv);
            r1h            = _mm_mul_pd(r1h, vscale);                        // U[1][2], U[1][3]
        }

        // Eliminate col 1 from rows 2–3
        // lo is already all L entries — only hi needs updating
        {
            __m128d f2 = _mm_unpackhi_pd(r2l, r2l); // broadcast L[2][1]
            __m128d f3 = _mm_unpackhi_pd(r3l, r3l);
            r2h        = _mm_sub_pd(r2h, _mm_mul_pd(f2, r1h));
            r3h        = _mm_sub_pd(r3h, _mm_mul_pd(f3, r1h));
        }
        // After this step: r_il fully final; r_ih = [L[i][2], partial[i][3]]

        // ── j=2: pivot = r2h[0]; scale lane 1 → U[2][3] ─────────────────────
        {
            double inv     = 1.0 / _mm_cvtsd_f64(r2h);                         // lane 0 = L[2][2]
            __m128d vscale = _mm_set1_pd(inv);
            r2h            = _mm_blend_pd(r2h, _mm_mul_pd(r2h, vscale), 0b10); // keep L[2][2], scale U[2][3]
        }

        // Eliminate col 2 from row 3
        // keep lane 0 of r3h (L[3][2]), update lane 1 → L[3][3]
        {
            __m128d f3 = _mm_movedup_pd(r3h); // broadcast L[3][2]
            r3h        = _mm_blend_pd(r3h, _mm_sub_pd(r3h, _mm_mul_pd(f3, r2h)), 0b10);
        }

        // ── j=3: L[3][3] = r3h[1], already final ─────────────────────────────

        Matrix4<double> LU = Matrix4<double>::zero();
        _mm_storeu_pd(&LU.elements[0], r0l);
        _mm_storeu_pd(&LU.elements[2], r0h);
        _mm_storeu_pd(&LU.elements[4], r1l);
        _mm_storeu_pd(&LU.elements[6], r1h);
        _mm_storeu_pd(&LU.elements[8], r2l);
        _mm_storeu_pd(&LU.elements[10], r2h);
        _mm_storeu_pd(&LU.elements[12], r3l);
        _mm_storeu_pd(&LU.elements[14], r3h);

        return Matrix4LU<double>{LU};
    }
} // namespace linalg

namespace linalg {
    namespace blas2 {
        auto matrix_vector_product(const Matrix4<float>& A, const Vector4<float> x) -> Vector4<float> {
            auto v_x = _mm_loadu_ps(x.elements);

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

            return Vector4<float>{_mm_cvtss_f32(v_product1_sum2),
                                  _mm_cvtss_f32(v_product2_sum2),
                                  _mm_cvtss_f32(v_product3_sum2),
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
            return Vector4<double>{A.r1c1 * x.x + A.r1c2 * x.y + A.r1c3 * x.z + A.r1c4 * x.w,
                                   A.r2c1 * x.x + A.r2c2 * x.y + A.r2c3 * x.z + A.r2c4 * x.w,
                                   A.r3c1 * x.x + A.r3c2 * x.y + A.r3c3 * x.z + A.r3c4 * x.w,
                                   A.r4c1 * x.x + A.r4c2 * x.y + A.r4c3 * x.z + A.r4c4 * x.w};
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
