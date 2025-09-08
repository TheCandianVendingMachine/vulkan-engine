#pragma once
#include "linalg/vector.h"

namespace simd {
    struct alignas(32) Pack2f {
        float x;
        float y;
    };

    struct alignas(32) Pack2d {
        double x;
        double y;
    };

    struct alignas(32) Pack4f {
        float x;
        float y;
        float z;
        float w;
    };

    struct alignas(32) Pack4d {
        double x;
        double y;
        double z;
        double w;
    };

    auto pack1(float v) -> Pack4f;
    auto pack2(float v) -> Pack2f;
    auto pack2_lower(float v) -> Pack4f;
    auto pack2_upper(float v) -> Pack4f;
    auto pack3_lower(float v) -> Pack4f;
    auto pack3_upper(float v) -> Pack4f;
    auto pack4(float v) -> Pack4f;
    auto pack_even(float v) -> Pack4f;
    auto pack_odd(float v) -> Pack4f;
    auto pack_lower(const linalg::Vector2<float>& v) -> Pack4f;
    auto pack_upper(const linalg::Vector2<float>& v) -> Pack4f;
    auto pack2(const linalg::Vector2<float>& v) -> Pack2f;
    auto pack(const linalg::Vector3<float>& v) -> Pack4f;
    auto pack(const linalg::Vector4<float>& v) -> Pack4f;
    auto spread_vec2(Pack2f &&pack) -> linalg::Vector2<float>;
    auto spread_vec2(Pack4f &&pack) -> std::pair<linalg::Vector2<float>, linalg::Vector2<float>>;
    auto spread_vec3(Pack4f &&pack) -> linalg::Vector3<float>;
    auto spread_vec4(Pack4f &&pack) -> linalg::Vector4<float>;

    auto pack1(double v) -> Pack4d;
    auto pack2(double v) -> Pack2d;
    auto pack2_lower(double v) -> Pack4d;
    auto pack2_upper(double v) -> Pack4d;
    auto pack3_lower(double v) -> Pack4d;
    auto pack3_upper(double v) -> Pack4d;
    auto pack4(double v) -> Pack4d;
    auto pack_even(double v) -> Pack4d;
    auto pack_odd(double v) -> Pack4d;
    auto pack_lower(const linalg::Vector2<double>& v) -> Pack4d;
    auto pack_upper(const linalg::Vector2<double>& v) -> Pack4d;
    auto pack2(const linalg::Vector2<double>& v) -> Pack2d;
    auto pack(const linalg::Vector3<double>& v) -> Pack4d;
    auto pack(const linalg::Vector4<double>& v) -> Pack4d;
    auto spread_vec2(Pack2d &&pack) -> linalg::Vector2<double>;
    auto spread_vec2(Pack4d &&pack) -> std::pair<linalg::Vector2<double>, linalg::Vector2<double>>;
    auto spread_vec3(Pack4d &&pack) -> linalg::Vector3<double>;
    auto spread_vec4(Pack4d &&pack) -> linalg::Vector4<double>;
}