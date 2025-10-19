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
auto axpy(const float a, const Vector2<float> x, const Vector2<float> y) -> Vector2<float> {
    auto v_a1   = (__m128*)(&a);
    auto v_a    = _mm_shuffle_ps(*v_a1, *v_a1, 0b0000'0000);
    auto v_x    = _mm_loadl_pi(__m128(), (__m64*)(x.elements));
    auto v_y    = _mm_loadl_pi(__m128(), (__m64*)(y.elements));
    auto v_ax   = _mm_mul_ps(v_a, v_x);

    auto v_axpy = _mm_add_ps(v_ax, v_y);

    float result[4];
    _mm_store_ps(result, v_axpy);
    return Vector2<float>{result[0], result[1]};
}

auto scale(const float a, const Vector2<float> x) -> Vector2<float> {
    auto v_a1 = (__m128*)(&a);
    auto v_a  = _mm_shuffle_ps(*v_a1, *v_a1, 0b0000'0000);
    auto v_x  = _mm_loadl_pi(__m128(), (__m64*)(x.elements));
    auto v_ax = _mm_mul_ps(v_a, v_x);

    float result[4];
    _mm_store_ps(result, v_ax);
    return Vector2<float>{result[0], result[1]};
}

auto copy(Vector2<float>& a, const Vector2<float> b) -> void {
    a = b;
}

auto swap(Vector2<float>& a, Vector2<float>& b) -> void {
    auto v_x = _mm_loadl_pi(__m128(), (__m64*)(a.elements));
    v_x      = _mm_loadh_pi(v_x, (__m64*)(b.elements));
    auto v_y = _mm_shuffle_ps(v_x, v_x, 0b0100'1110);

    v_x      = _mm_xor_ps(v_y, v_x);
    v_y      = _mm_xor_ps(v_x, v_y);
    v_x      = _mm_xor_ps(v_y, v_x);

    float result[4];
    _mm_store_ps(result, v_x);

    std::memcpy(a.elements, &result[0], 2 * sizeof(float));
    std::memcpy(b.elements, &result[2], 2 * sizeof(float));
}

auto dot(const Vector2<float> a, const Vector2<float> b) -> float {
    auto v_a   = _mm_loadl_pi(__m128(), (__m64*)(a.elements));
    auto v_b   = _mm_loadl_pi(__m128(), (__m64*)(b.elements));

    auto v_ab  = _mm_mul_ps(v_a, v_b);
    auto v_ba  = _mm_shuffle_ps(v_ab, v_ab, 0b0001'0001);
    auto v_dot = _mm_add_ps(v_ab, v_ba);
    return _mm_cvtss_f32(v_dot);
}

/* Single vector operations are quicker when not using SSE */
auto component_sum(const Vector2<float> x) -> float {
    return std::abs(x.x) + std::abs(x.y);
}

auto magnitude(const Vector2<float> x) -> float {
    return std::sqrt(x.x * x.x + x.y * x.y);
}

auto component_max(const Vector2<float> x) -> float {
    return std::fmaxf(std::abs(x.x), std::abs(x.y));
}
} // namespace blas1
} // namespace linalg

namespace linalg {
namespace blas1 {
auto axpy(const double a, const Vector2<double> x, const Vector2<double> y) -> Vector2<double> {
    auto v_a1   = (__m128d*)(&a);
    auto v_a    = _mm_shuffle_pd(*v_a1, *v_a1, 0b00);
    auto v_x    = _mm_loadu_pd(x.elements);
    auto v_y    = _mm_loadu_pd(y.elements);
    auto v_ax   = _mm_mul_pd(v_a, v_x);

    auto v_axpy = _mm_add_pd(v_ax, v_y);

    double result[2];
    _mm_store_pd(result, v_axpy);
    return Vector2<double>{result[0], result[1]};
}

auto scale(const double a, const Vector2<double> x) -> Vector2<double> {
    auto v_a1 = (__m128d*)(&a);
    auto v_a  = _mm_shuffle_pd(*v_a1, *v_a1, 0b00);
    auto v_x  = _mm_loadu_pd(x.elements);
    auto v_ax = _mm_mul_pd(v_a, v_x);

    double result[2];
    _mm_store_pd(result, v_ax);
    return Vector2<double>{result[0], result[1]};
}

auto copy(Vector2<double>& a, const Vector2<double> b) -> void {
    a = b;
}

auto swap(Vector2<double>& a, Vector2<double>& b) -> void {
    auto v_x = _mm_loadu_pd(a.elements);
    auto v_y = _mm_loadu_pd(b.elements);

    v_x      = _mm_xor_pd(v_y, v_x);
    v_y      = _mm_xor_pd(v_x, v_y);
    v_x      = _mm_xor_pd(v_y, v_x);

    double result_a[2];
    double result_b[2];
    _mm_store_pd(result_a, v_x);
    _mm_store_pd(result_b, v_y);

    std::memcpy(a.elements, result_a, 2 * sizeof(double));
    std::memcpy(b.elements, result_b, 2 * sizeof(double));
}

auto dot(const Vector2<double> a, const Vector2<double> b) -> double {
    auto v_a   = _mm_loadu_pd(a.elements);
    auto v_b   = _mm_loadu_pd(b.elements);

    auto v_ab  = _mm_mul_pd(v_a, v_b);
    auto v_ba  = _mm_shuffle_pd(v_ab, v_ab, 0b0001'0001);
    auto v_dot = _mm_add_pd(v_ab, v_ba);
    return _mm_cvtsd_f64(v_dot);
}

/* Single vector operations are quicker when not using SSE */
auto component_sum(const Vector2<double> x) -> double {
    return std::abs(x.x) + std::abs(x.y);
}

auto magnitude(const Vector2<double> x) -> double {
    return std::sqrt(x.x * x.x + x.y * x.y);
}

auto component_max(const Vector2<double> x) -> double {
    return std::fmax(std::abs(x.x), std::abs(x.y));
}
} // namespace blas1
} // namespace linalg
