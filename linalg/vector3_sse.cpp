#include "linalg/vector_ops.h"
#include <intrin.h>
#include <cstring>
#include <cmath>

constexpr auto float_abs_bits() -> __m128i {
    int32_t bitmask = 0x7FFFFFFF;
    int32_t uniforms[4] = {
        bitmask,
        bitmask,
        bitmask,
        bitmask
    };
    return *(__m128i*)(&uniforms);
}

constexpr auto double_abs_bits() -> __m128d {
    union Uniform {
        int64_t i;
        double real;
    } uniform = Uniform {
        0x7FFFFFFFFFFFFFFF
    };
    return *(__m128d*)(&uniform.real);
}

namespace linalg {
    namespace blas1 {
        auto axpy(const float a, const Vector3<float> x, const Vector3<float> y) -> Vector3<float> {
            auto v_a1 = (__m128*)(&a);
            auto v_a = _mm_shuffle_ps(*v_a1, *v_a1, 0b00'00'00'00);
            auto v_x = _mm_loadl_pi(__m128(), (__m64*)(x.elements));
            auto v_y = _mm_loadl_pi(__m128(), (__m64*)(y.elements));
            auto v_ax = _mm_mul_ps(v_a, v_x);

            auto v_axpy = _mm_add_ps(v_ax, v_y);

            float result[4];
            _mm_store_ps(result, v_axpy);
            return Vector3<float>{ result[0], result[1], a * x.z + y.z };
        }

        auto scale(const float a, const Vector3<float> x) -> Vector3<float> {
            auto v_a1 = (__m128*)(&a);
            auto v_a = _mm_shuffle_ps(*v_a1, *v_a1, 0b00'00'00'00);
            auto v_x = _mm_loadl_pi(__m128(), (__m64*)(x.elements));
            auto v_ax = _mm_mul_ps(v_a, v_x);

            float result[4];
            _mm_store_ps(result, v_ax);
            return Vector3<float>{ result[0], result[1], a * x.z };
        }

        auto copy(Vector3<float>& a, const Vector3<float> b) -> void {
            a = b;
        }

        auto swap(Vector3<float>& a, Vector3<float>& b) -> void {
            auto v_x = _mm_loadu_ps(a.elements);
            auto v_y = _mm_loadu_ps(b.elements);

            v_x = _mm_xor_ps(v_y, v_x);
            v_y = _mm_xor_ps(v_x, v_y);
            v_x = _mm_xor_ps(v_y, v_x);

            float result_a[4];
            float result_b[4];
            _mm_store_ps(result_a, v_x);
            _mm_store_ps(result_b, v_y);

            std::memcpy(a.elements, result_a, 3 * sizeof(float));
            std::memcpy(b.elements, result_b, 3 * sizeof(float));
        }

        auto dot(const Vector3<float> a, const Vector3<float> b) -> float {
            auto v_a = _mm_loadl_pi(__m128(), (__m64*)(a.elements));
            auto v_b = _mm_loadl_pi(__m128(), (__m64*)(b.elements));

            auto v_ab = _mm_mul_ps(v_a, v_b);
            auto v_ba = _mm_shuffle_ps(v_ab, v_ab, 0b00'01'00'01);
            auto v_dot = _mm_add_ps(v_ab, v_ba);
            return _mm_cvtss_f32(v_dot) + a.z * b.z;
        }

        /* Single vector operations are quicker when not using SSE */
        auto component_sum(const Vector3<float> x) -> float {
            return std::abs(x.x) + std::abs(x.y) + std::abs(x.z);
        }

        auto magnitude(const Vector3<float> x) -> float {
            return std::sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
        }

        auto component_max(const Vector3<float> x) -> float {
            return std::fmaxf(std::abs(x.x), std::fmaxf(std::abs(x.y), std::abs(x.z)));
        }
    }
}