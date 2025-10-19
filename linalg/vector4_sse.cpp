#include "linalg/vector_ops.h"
#ifdef __GNUG__
    #include <immintrin.h>
#else
    #include <intrin.h>
#endif
#include <cmath>
#include <cstring>

constexpr auto float_abs_bits() -> __m128i {
    int32_t bitmask     = 0x7FFFFFFF;
    int32_t uniforms[4] = {bitmask, bitmask, bitmask, bitmask};
    return *(__m128i*)(&uniforms);
}

constexpr auto double_abs_bits() -> __m128d {
    union Uniform {
            int64_t i;
            double real;
    } uniform = Uniform{0x7FFFFFFFFFFFFFFF};
    return *(__m128d*)(&uniform.real);
}

namespace linalg {
namespace blas1 {
auto axpy(const float a, const Vector4<float> x, const Vector4<float> y) -> Vector4<float> {
    auto v_a1    = (__m128*)(&a);
    auto v_a     = _mm_shuffle_ps(*v_a1, *v_a1, 0b0000'0000);
    auto v_xl    = _mm_loadl_pi(__m128(), (__m64*)(x.elements + 0));
    auto v_yl    = _mm_loadl_pi(__m128(), (__m64*)(y.elements + 0));
    auto v_axl   = _mm_mul_ps(v_a, v_xl);

    auto v_xh    = _mm_loadh_pi(__m128(), (__m64*)(x.elements + 2));
    auto v_yh    = _mm_loadh_pi(__m128(), (__m64*)(y.elements + 2));
    auto v_axh   = _mm_mul_ps(v_a, v_xh);

    auto v_axpyl = _mm_add_ps(v_axl, v_yl);
    auto v_axpyh = _mm_add_ps(v_axh, v_yh);

    auto v_axpy  = _mm_add_ps(v_axpyh, v_axpyl);

    float result[4];
    _mm_store_ps(result, v_axpy);
    return Vector4<float>{result[0], result[1], result[2], result[3]};
}

auto scale(const float a, const Vector4<float> x) -> Vector4<float> {
    auto v_a1  = (__m128*)(&a);
    auto v_a   = _mm_shuffle_ps(*v_a1, *v_a1, 0b0000'0000);
    auto v_xl  = _mm_loadl_pi(__m128(), (__m64*)(x.elements));
    auto v_axl = _mm_mul_ps(v_a, v_xl);
    auto v_xh  = _mm_loadh_pi(__m128(), (__m64*)(x.elements + 2));
    auto v_axh = _mm_mul_ps(v_a, v_xh);

    auto v_ax  = _mm_add_ps(v_axh, v_axl);

    float result[4];
    _mm_store_ps(result, v_ax);
    return Vector4<float>{result[0], result[1], result[2], result[3]};
}

auto copy(Vector4<float>& a, const Vector4<float> b) -> void {
    a = b;
}

auto swap(Vector4<float>& a, Vector4<float>& b) -> void {
    auto v_x = _mm_loadu_ps(a.elements);
    auto v_y = _mm_loadu_ps(b.elements);

    v_x      = _mm_xor_ps(v_y, v_x);
    v_y      = _mm_xor_ps(v_x, v_y);
    v_x      = _mm_xor_ps(v_y, v_x);

    float result_a[4];
    float result_b[4];
    _mm_store_ps(result_a, v_x);
    _mm_store_ps(result_b, v_y);

    std::memcpy(a.elements, result_a, 4 * sizeof(float));
    std::memcpy(b.elements, result_b, 4 * sizeof(float));
}

auto dot(const Vector4<float> a, const Vector4<float> b) -> float {
    auto v_al   = _mm_loadl_pi(__m128(), (__m64*)(a.elements + 0));
    auto v_bl   = _mm_loadl_pi(__m128(), (__m64*)(b.elements + 0));
    auto v_abl  = _mm_mul_ps(v_al, v_bl);

    auto v_ah   = _mm_loadl_pi(__m128(), (__m64*)(a.elements + 2));
    auto v_bh   = _mm_loadl_pi(__m128(), (__m64*)(b.elements + 2));
    auto v_abh  = _mm_mul_ps(v_ah, v_bh);

    auto v_bal  = _mm_shuffle_ps(v_abl, v_abl, 0b0001'0001);
    auto v_dotl = _mm_add_ps(v_abl, v_bal);

    auto v_bah  = _mm_shuffle_ps(v_abh, v_abh, 0b0001'0001);
    auto v_doth = _mm_add_ps(v_abh, v_bah);

    return _mm_cvtss_f32(v_doth) + _mm_cvtss_f32(v_dotl);
}

/* Single vector operations are quicker when not using SSE */
auto component_sum(const Vector4<float> x) -> float {
    return std::abs(x.x) + std::abs(x.y) + std::abs(x.z) + std::abs(x.w);
}

auto magnitude(const Vector4<float> x) -> float {
    return std::sqrt(x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w);
}

auto component_max(const Vector4<float> x) -> float {
    return std::fmaxf(std::fmaxf(std::abs(x.x), std::abs(x.y)), std::fmaxf(std::abs(x.z), std::abs(x.w)));
}
} // namespace blas1
} // namespace linalg

namespace linalg {
namespace blas1 {
auto axpy(const double a, const Vector4<double> x, const Vector4<double> y) -> Vector4<double> {
    auto v_a1    = (__m128d*)(&a);
    auto v_a     = _mm_shuffle_pd(*v_a1, *v_a1, 0b00);

    auto v_xl    = _mm_loadu_pd(x.elements + 0);
    auto v_yl    = _mm_loadu_pd(y.elements + 0);
    auto v_axl   = _mm_mul_pd(v_a, v_xl);

    auto v_xh    = _mm_loadu_pd(x.elements + 2);
    auto v_yh    = _mm_loadu_pd(y.elements + 2);
    auto v_axh   = _mm_mul_pd(v_a, v_xh);

    auto v_axpyl = _mm_add_pd(v_axl, v_yl);
    auto v_axpyh = _mm_add_pd(v_axh, v_yh);

    double result_l[2];
    double result_h[2];
    _mm_store_pd(result_l, v_axpyl);
    _mm_store_pd(result_h, v_axpyh);
    return Vector4<double>{result_l[0], result_l[1], result_h[0], result_h[1]};
}

auto scale(const double a, const Vector4<double> x) -> Vector4<double> {
    auto v_a1  = (__m128d*)(&a);
    auto v_a   = _mm_shuffle_pd(*v_a1, *v_a1, 0b00);
    auto v_xl  = _mm_loadu_pd(x.elements + 0);
    auto v_axl = _mm_mul_pd(v_a, v_xl);

    auto v_xh  = _mm_loadu_pd(x.elements + 2);
    auto v_axh = _mm_mul_pd(v_a, v_xh);

    double result_l[2];
    double result_h[2];
    _mm_store_pd(result_l, v_axl);
    _mm_store_pd(result_h, v_axh);
    return Vector4<double>{result_l[0], result_l[1], result_h[0], result_h[1]};
}

auto copy(Vector4<double>& a, const Vector4<double> b) -> void {
    a = b;
}

auto swap(Vector4<double>& a, Vector4<double>& b) -> void {
    auto v_xl = _mm_loadu_pd(a.elements + 0);
    auto v_yl = _mm_loadu_pd(b.elements + 0);
    auto v_xh = _mm_loadu_pd(a.elements + 2);
    auto v_yh = _mm_loadu_pd(b.elements + 2);

    v_xl      = _mm_xor_pd(v_yl, v_xl);
    v_yl      = _mm_xor_pd(v_xl, v_yl);
    v_xl      = _mm_xor_pd(v_yl, v_xl);

    v_xh      = _mm_xor_pd(v_yh, v_xh);
    v_yh      = _mm_xor_pd(v_xh, v_yh);
    v_xh      = _mm_xor_pd(v_yh, v_xh);

    double result_a_l[2];
    double result_b_l[2];
    _mm_store_pd(result_a_l, v_xl);
    _mm_store_pd(result_b_l, v_yl);

    double result_a_h[2];
    double result_b_h[2];
    _mm_store_pd(result_a_h, v_xh);
    _mm_store_pd(result_b_h, v_yh);

    std::memcpy(a.elements + 0, result_a_l, 2 * sizeof(double));
    std::memcpy(b.elements + 0, result_b_l, 2 * sizeof(double));

    std::memcpy(a.elements + 2, result_a_h, 2 * sizeof(double));
    std::memcpy(b.elements + 2, result_b_h, 2 * sizeof(double));
}

auto dot(const Vector4<double> a, const Vector4<double> b) -> double {
    auto v_al   = _mm_loadu_pd(a.elements + 0);
    auto v_bl   = _mm_loadu_pd(b.elements + 0);
    auto v_ah   = _mm_loadu_pd(a.elements + 2);
    auto v_bh   = _mm_loadu_pd(b.elements + 2);

    auto v_abl  = _mm_mul_pd(v_al, v_bl);
    auto v_bal  = _mm_shuffle_pd(v_abl, v_abl, 0b0001'0001);
    auto v_dotl = _mm_add_pd(v_abl, v_bal);

    auto v_abh  = _mm_mul_pd(v_ah, v_bh);
    auto v_bah  = _mm_shuffle_pd(v_abh, v_abh, 0b0001'0001);
    auto v_doth = _mm_add_pd(v_abh, v_bah);

    return _mm_cvtsd_f64(v_dotl) + _mm_cvtsd_f64(v_doth);
}

/* Single vector operations are quicker when not using SSE */
auto component_sum(const Vector4<double> x) -> double {
    return std::abs(x.x) + std::abs(x.y) + std::abs(x.z) + std::abs(x.w);
}

auto magnitude(const Vector4<double> x) -> double {
    return std::sqrt(x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w);
}

auto component_max(const Vector4<double> x) -> double {
    return std::fmax(std::fmax(std::abs(x.x), std::abs(x.y)), std::fmax(std::abs(x.z), std::abs(x.w)));
}
} // namespace blas1
} // namespace linalg
