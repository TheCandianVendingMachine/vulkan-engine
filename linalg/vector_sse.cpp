#include "linalg/vector_ops.h"
#include "linalg/simd.h"
#include <intrin.h>
#include <cstring>
#include <cmath>

constexpr auto float_abs_bits() -> float {
    union Uniform {
        int32_t i;
        float real;
    } uniform = Uniform {
        0x7FFFFFFF
    };
    return uniform.real;
}

constexpr auto double_abs_bits() -> double {
    union Uniform {
        int64_t i;
        double real;
    } uniform = Uniform {
        0x7FFFFFFFFFFFFFFF
    };
    return uniform.real;
}

namespace linalg {
    namespace blas1 {
        auto axpy(float a, const Vector2<float>& x, Vector2<float>& y) -> Vector2<float> {
            auto a_pack = simd::pack2_lower(a);
            auto v_a = _mm_load_ps(&a_pack.x);

            auto x_pack = simd::pack_lower(x);
            auto v_x = _mm_load_ps(&x_pack.x);
            auto v_ax = _mm_mul_ps(v_a, v_x);

            auto y_pack = simd::pack_lower(y);
            auto v_y = _mm_load_ps(&y_pack.x);

            auto v_axpy = _mm_add_ps(v_ax, v_y);

            auto axpy = simd::Pack4f{};
            _mm_store_ps(&axpy.x, v_axpy);

            return simd::spread_vec2(std::move(axpy)).first;
        }

        auto axpy2(float a[2], const Vector2<float> x[2], Vector2<float> y[2]) -> std::pair<Vector2<float>, Vector2<float>> {
            auto a_pack = simd::Pack4f {
                a[0], a[0],
                a[1], a[1]
            };
            auto v_a = _mm_load_ps(&a_pack.x);
            auto x_pack = simd::Pack4f {
                x[0].x, x[0].y,
                x[1].x, x[1].y
            };
            auto v_x = _mm_load_ps(&x_pack.x);
            auto v_ax = _mm_mul_ps(v_a, v_x);

            auto y_pack = simd::Pack4f {
                y[0].x, y[0].y,
                y[1].x, y[1].y
            };
            auto v_y = _mm_load_ps(&y_pack.x);

            auto v_axpy = _mm_add_ps(v_ax, v_y);

            auto axpy = simd::Pack4f{};
            _mm_store_ps(&axpy.x, v_axpy);

            return simd::spread_vec2(std::move(axpy));
        }

        auto scale(float a, const Vector2<float>& x) -> Vector2<float> {
            auto a_pack = simd::pack4(a);
            auto v_a = _mm_load_ps(&a_pack.x);
            auto x_pack = simd::pack_lower(x);
            auto v_x = _mm_load_ps(&x_pack.x);

            auto v_ax = _mm_mul_ps(v_a, v_x);

            auto ax = simd::Pack4f{};
            _mm_store_ps(&ax.x, v_ax);
            return simd::spread_vec2(std::move(ax)).first;
        }

        auto scale2(float a[2], const Vector2<float> x[2]) -> std::pair<Vector2<float>, Vector2<float>> {
            auto a_pack = simd::Pack4f {
                a[0], a[0],
                a[1], a[1]
            };
            auto v_a = _mm_load_ps(&a_pack.x);
            auto x_pack = simd::Pack4f {
                x[0].x, x[0].y,
                x[1].x, x[1].y
            };
            auto v_x = _mm_load_ps(&x_pack.x);

            auto v_ax = _mm_mul_ps(v_a, v_x);

            auto ax = simd::Pack4f{};
            _mm_store_ps(&ax.x, v_ax);
            return simd::spread_vec2(std::move(ax));
        }

        auto copy(Vector2<float>& a, const Vector2<float>& b) -> void {
            std::memcpy(&a.x, &b.x, sizeof(a.elements));
        }

        auto swap(Vector2<float>& a, Vector2<float>& b) -> void {
            auto a_pack = simd::Pack4f {
                a.x,
                a.y,
                b.x,
                b.y
            };
            auto v_x = _mm_load_ps(&a_pack.x);
            auto v_y = _mm_shuffle_ps(v_x, v_x, 0b01'00'11'10);

            v_x = _mm_xor_ps(v_y, v_x);
            v_y = _mm_xor_ps(v_x, v_y);
            v_x = _mm_xor_ps(v_y, v_x);

            _mm_store_ps(&a_pack.x, v_x);

            std::memcpy(&a.x, &a_pack.x, 2 * sizeof(float));
            std::memcpy(&b.x, &a_pack.z, 2 * sizeof(float));
        }

        auto dot(Vector2<float>& a, const Vector2<float>& b) -> float {
            auto a_pack = simd::pack_lower(a);
            auto v_a = _mm_load_ps(&a_pack.x);
            auto b_pack = simd::pack_lower(b);
            auto v_b = _mm_load_ps(&b_pack.x);

            auto v_ab = _mm_mul_ps(v_a, v_b);
            auto v_ba = _mm_shuffle_ps(v_ab, v_ab, 0b00'01'00'01);
            auto v_dot = _mm_add_ps(v_ab, v_ba);
            return _mm_cvtss_f32(v_dot);
        }

        auto component_sum(Vector2<float>& x) -> float {
            auto v_sign_bit = _mm_set_ps1(float_abs_bits());
            auto a_pack = simd::pack_lower(x);
            auto v_a = _mm_load_ps(&a_pack.x);

            auto v_abs = _mm_and_ps(v_a, v_sign_bit);
            auto v_a_shuf = _mm_shuffle_ps(v_abs, v_abs, 0b00'01'00'01);
            auto v_sum = _mm_add_ps(v_abs, v_a_shuf);
            return _mm_cvtss_f32(v_sum);
        }

        auto magnitude(Vector2<float>& x) -> float {
            auto a_pack = simd::pack_lower(x);
            auto v_a = _mm_load_ps(&a_pack.x);
            auto v_a2 = _mm_mul_ps(v_a, v_a);
            auto v_a_shuf = _mm_shuffle_ps(v_a2, v_a2, 0b00'01'00'01);
            auto v_sum = _mm_add_ps(v_a2, v_a_shuf);
            auto v_sqrt = _mm_sqrt_ss(v_sum);
            return _mm_cvtss_f32(v_sqrt);
        }

        auto component_max(Vector2<float>& x) -> float {
            auto x_pack = simd::pack_lower(x);
            auto v_x = _mm_load_ps(&x_pack.x);
            auto v_x_shuf = _mm_shuffle_ps(v_x, v_x, 0b00'01'00'01);
            auto v_max = _mm_max_ps(v_x, v_x_shuf);
            return _mm_cvtss_f32(v_max);
        }
    }
}


namespace linalg {
    namespace blas1 {
        auto axpy(double a, const Vector2<double>& x, Vector2<double>& y) -> Vector2<double> {
            auto a_pack = simd::pack2(a);
            auto v_a = _mm_load_pd(&a_pack.x);

            auto x_pack = simd::pack2(x);
            auto v_x = _mm_load_pd(&x_pack.x);
            auto v_ax = _mm_mul_pd(v_a, v_x);

            auto y_pack = simd::pack2(y);
            auto v_y = _mm_load_pd(&y_pack.x);

            auto v_axpy = _mm_add_pd(v_ax, v_y);

            auto axpy = simd::Pack2d{};
            _mm_store_pd(&axpy.x, v_axpy);

            return simd::spread_vec2(std::move(axpy));
        }

        auto scale(double a, const Vector2<double>& x) -> Vector2<double> {
            auto a_pack = simd::pack2(a);
            auto v_a = _mm_load_pd(&a_pack.x);
            auto x_pack = simd::pack2(x);
            auto v_x = _mm_load_pd(&x_pack.x);

            auto v_ax = _mm_mul_pd(v_a, v_x);

            auto ax = simd::Pack2d{};
            _mm_store_pd(&ax.x, v_ax);
            return simd::spread_vec2(std::move(ax));
        }

        auto copy(Vector2<double>& a, const Vector2<double>& b) -> void {
            std::memcpy(&a.x, &b.x, sizeof(a.elements));
        }

        auto swap(Vector2<double>& a, Vector2<double>& b) -> void {
            auto a_pack = simd::pack2(a);
            auto v_x = _mm_load_pd(&a_pack.x);
            auto b_pack = simd::pack2(a);
            auto v_y = _mm_load_pd(&b_pack.x);

            v_x = _mm_xor_pd(v_y, v_x);
            v_y = _mm_xor_pd(v_x, v_y);
            v_x = _mm_xor_pd(v_y, v_x);

            _mm_store_pd(&a_pack.x, v_x);
            _mm_store_pd(&b_pack.x, v_y);

            std::memcpy(&a.x, &a_pack.x, 2 * sizeof(double));
            std::memcpy(&b.x, &b_pack.x, 2 * sizeof(double));
        }

        auto dot(Vector2<double>& a, const Vector2<double>& b) -> double {
            auto a_pack = simd::pack2(a);
            auto v_a = _mm_load_pd(&a_pack.x);
            auto b_pack = simd::pack2(b);
            auto v_b = _mm_load_pd(&b_pack.x);

            auto v_ab = _mm_mul_pd(v_a, v_b);
            auto v_ba = _mm_shuffle_pd(v_ab, v_ab, 0b01);
            auto v_dot = _mm_add_pd(v_ab, v_ba);
            return _mm_cvtsd_f64(v_dot);
        }

        auto component_sum(Vector2<double>& x) -> double {
            auto v_sign_bit = _mm_set_pd1(double_abs_bits());
            auto a_pack = simd::pack2(x);
            auto v_a = _mm_load_pd(&a_pack.x);

            auto v_abs = _mm_and_pd(v_a, v_sign_bit);
            auto v_a_shuf = _mm_shuffle_pd(v_abs, v_abs, 0b01);
            auto v_sum = _mm_add_pd(v_abs, v_a_shuf);
            return _mm_cvtsd_f64(v_sum);
        }

        auto magnitude(Vector2<double>& x) -> double {
            auto a_pack = simd::pack2(x);
            auto v_a = _mm_load_pd(&a_pack.x);
            auto v_a2 = _mm_mul_pd(v_a, v_a);
            auto v_a_shuf = _mm_shuffle_pd(v_a2, v_a2, 0b01);
            auto v_sum = _mm_add_pd(v_a2, v_a_shuf);
            auto v_sqrt = _mm_sqrt_pd(v_sum);
            return _mm_cvtsd_f64(v_sqrt);
        }

        auto component_max(Vector2<double>& x) -> double {
            auto x_pack = simd::pack2(x);
            auto v_x = _mm_load_pd(&x_pack.x);
            auto v_x_shuf = _mm_shuffle_pd(v_x, v_x, 0b01);
            auto v_max = _mm_max_pd(v_x, v_x_shuf);
            return _mm_cvtsd_f64(v_max);
        }
    }
}

