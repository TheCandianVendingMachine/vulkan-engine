#include "linalg/vector_ops.h"
#ifdef __GNUG__
    #include <immintrin.h>
#else
    #include <intrin.h>
#endif
#include <cmath>
#include <cstring>
#include <utility>

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
        auto axpy(const float a, const Vector3<float> x, const Vector3<float> y) -> Vector3<float> {
            auto v_a  = _mm_set1_ps(a);
            auto v_x  = _mm_loadl_pi(_mm_setzero_ps(), reinterpret_cast<const __m64*>(x.elements));
            auto v_y  = _mm_loadl_pi(_mm_setzero_ps(), reinterpret_cast<const __m64*>(y.elements));
            auto v_ax = _mm_mul_ps(v_a, v_x);

            auto v_axpy = _mm_add_ps(v_ax, v_y);

            float result[4];
            _mm_storeu_ps(result, v_axpy);
            return Vector3<float>{result[0], result[1], a * x.z + y.z};
        }

        auto scale(const float a, const Vector3<float> x) -> Vector3<float> {
            auto v_a  = _mm_set1_ps(a);
            auto v_x  = _mm_loadl_pi(_mm_setzero_ps(), reinterpret_cast<const __m64*>(x.elements));
            auto v_ax = _mm_mul_ps(v_a, v_x);

            float result[4];
            _mm_storeu_ps(result, v_ax);
            return Vector3<float>{result[0], result[1], a * x.z};
        }

        auto copy(Vector3<float>& a, const Vector3<float> b) -> void {
            a = b;
        }

        auto swap(Vector3<float>& a, Vector3<float>& b) -> void {
            std::swap(a.x, b.x);
            std::swap(a.y, b.y);
            std::swap(a.z, b.z);
        }

        auto dot(const Vector3<float> a, const Vector3<float> b) -> float {
            auto v_a = _mm_loadl_pi(__m128(), (__m64*)(a.elements));
            auto v_b = _mm_loadl_pi(__m128(), (__m64*)(b.elements));

            auto v_ab  = _mm_mul_ps(v_a, v_b);
            auto v_ba  = _mm_shuffle_ps(v_ab, v_ab, 0b0001'0001);
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
    } // namespace blas1
} // namespace linalg

namespace linalg {
    namespace blas1 {
        auto axpy(const double a, const Vector3<double> x, const Vector3<double> y) -> Vector3<double> {
            auto v_a  = _mm_set1_pd(a);
            auto v_x  = _mm_loadu_pd(x.elements);
            auto v_y  = _mm_loadu_pd(y.elements);
            auto v_ax = _mm_mul_pd(v_a, v_x);

            auto v_axpy = _mm_add_pd(v_ax, v_y);

            double result[2];
            _mm_storeu_pd(result, v_axpy);
            return Vector3<double>{result[0], result[1], a * x.z + y.z};
        }

        auto scale(const double a, const Vector3<double> x) -> Vector3<double> {
            auto v_a  = _mm_set1_pd(a);
            auto v_x  = _mm_loadu_pd(x.elements);
            auto v_ax = _mm_mul_pd(v_a, v_x);

            double result[2];
            _mm_storeu_pd(result, v_ax);
            return Vector3<double>{result[0], result[1], a * x.z};
        }

        auto copy(Vector3<double>& a, const Vector3<double> b) -> void {
            a = b;
        }

        auto swap(Vector3<double>& a, Vector3<double>& b) -> void {
            auto v_x = _mm_loadu_pd(a.elements);
            auto v_y = _mm_loadu_pd(b.elements);

            v_x = _mm_xor_pd(v_y, v_x);
            v_y = _mm_xor_pd(v_x, v_y);
            v_x = _mm_xor_pd(v_y, v_x);

            double result_a[2];
            double result_b[2];
            _mm_storeu_pd(result_a, v_x);
            _mm_storeu_pd(result_b, v_y);

            std::memcpy(a.elements, result_a, 2 * sizeof(double));
            std::memcpy(b.elements, result_b, 2 * sizeof(double));
            std::swap(a.z, b.z);
        }

        auto dot(const Vector3<double> a, const Vector3<double> b) -> double {
            auto v_a = _mm_loadu_pd(a.elements);
            auto v_b = _mm_loadu_pd(b.elements);

            auto v_ab  = _mm_mul_pd(v_a, v_b);
            auto v_ba  = _mm_shuffle_pd(v_ab, v_ab, _MM_SHUFFLE2(0, 1));
            auto v_dot = _mm_add_pd(v_ab, v_ba);
            return _mm_cvtsd_f64(v_dot) + a.z * b.z;
        }

        /* Single vector operations are quicker when not using SSE */
        auto component_sum(const Vector3<double> x) -> double {
            return std::abs(x.x) + std::abs(x.y) + std::abs(x.z);
        }

        auto magnitude(const Vector3<double> x) -> double {
            return std::sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
        }

        auto component_max(const Vector3<double> x) -> double {
            return std::fmax(std::abs(x.x), std::fmax(std::abs(x.y), std::abs(x.z)));
        }
    } // namespace blas1
} // namespace linalg
