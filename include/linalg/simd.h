#pragma once
#include "linalg/vector.h"

namespace simd {
    struct alignas(32) Pack2f {
        union {
            float xmm[2];
            struct {
                float x;
                float y;
            };
        };
    };

    struct alignas(32) Pack2d {
        union {
            double xmm[2];
            struct {
                double x;
                double y;
            };
        };
    };

    struct alignas(32) Pack4f {
        union {
            float xmm[4];
            struct {
                float x;
                float y;
                float z;
                float w;
            };
        };
    };

    struct alignas(32) Pack4d {
        union {
            double xmm[4];
            struct {
                double x;
                double y;
                double z;
                double w;
            };
        };
    };

    constexpr inline auto pack1(float v) -> Pack4f { return Pack4f{ v, 0.f, 0.f, 0.f }; }
    constexpr inline auto pack2(float v) -> Pack2f { return Pack2f{ v, v }; }
    constexpr inline auto pack2_lower(float v) -> Pack4f { return Pack4f{ v, v, 0.f, 0.f }; }
    constexpr inline auto pack2_upper(float v) -> Pack4f { return Pack4f{ 0.f, 0.f, v, v }; }
    constexpr inline auto pack3_lower(float v) -> Pack4f { return Pack4f{ v, v, v, 0.f }; }
    constexpr inline auto pack3_upper(float v) -> Pack4f { return Pack4f{ 0.f, v, v, v }; }
    constexpr inline auto pack4(float v) -> Pack4f { return Pack4f{ v, v, v, v }; }
    constexpr inline auto pack_even(float v) -> Pack4f { return Pack4f{ v, 0.f, v, 0.f }; }
    constexpr inline auto pack_odd(float v) -> Pack4f { return Pack4f { 0.f, v, 0.f, v }; }
    constexpr inline auto pack_lower(const linalg::Vector2<float>& v) -> Pack4f { return Pack4f{ v.x, v.y, 0.f, 0.f }; }
    constexpr inline auto pack_upper(const linalg::Vector2<float>& v) -> Pack4f { return Pack4f{ 0.f, 0.f, v.x, v.y }; }
    constexpr inline auto pack2(const linalg::Vector2<float>& v) -> Pack2f { return Pack2f{ v.x, v.y }; }
    constexpr inline auto pack(const linalg::Vector3<float>& v) -> Pack4f { return Pack4f{ v.x, v.y, v.z, 0.f }; }
    constexpr inline auto pack(const linalg::Vector4<float>& v) -> Pack4f { return Pack4f{ v.x, v.y, v.z, v.w }; }
    auto spread_vec2(Pack2f &&pack) -> linalg::Vector2<float> { return linalg::Vector2<float>{ pack.x, pack.y }; }
    auto spread_vec2(Pack4f &&pack) -> std::pair<linalg::Vector2<float>, linalg::Vector2<float>> { return { linalg::Vector2<float>{ pack.x, pack.y }, linalg::Vector2<float>{ pack.z, pack.w } }; }
    auto spread_vec3(Pack4f &&pack) -> linalg::Vector3<float> { return linalg::Vector3<float>{ pack.x, pack.y, pack.z }; }
    auto spread_vec4(Pack4f &&pack) -> linalg::Vector4<float> { return linalg::Vector4<float>{ pack.x, pack.y, pack.z, pack.w }; }

    constexpr inline auto pack1(double v) -> Pack4d { return Pack4d{ v, 0.0, 0.0, 0.0 }; }
    constexpr inline auto pack2(double v) -> Pack2d { return Pack2d{ v, v }; }
    constexpr inline auto pack2_lower(double v) -> Pack4d { return Pack4d{ v, v, 0.0, 0.0 }; }
    constexpr inline auto pack2_upper(double v) -> Pack4d { return Pack4d{ 0.0, 0.0, v, v }; }
    constexpr inline auto pack3_lower(double v) -> Pack4d { return Pack4d{ v, v, v, 0.0 }; }
    constexpr inline auto pack3_upper(double v) -> Pack4d { return Pack4d{ 0.0, v, v, v }; }
    constexpr inline auto pack4(double v) -> Pack4d { return Pack4d{ v, v, v, v }; }
    constexpr inline auto pack_even(double v) -> Pack4d { return Pack4d{ v, 0.0, v, 0.0 }; }
    constexpr inline auto pack_odd(double v) -> Pack4d { return Pack4d { 0.0, v, 0.0, v }; }
    constexpr inline auto pack_lower(const linalg::Vector2<double>& v) -> Pack4d { return Pack4d{ v.x, v.y, 0.0, 0.0 }; }
    constexpr inline auto pack_upper(const linalg::Vector2<double>& v) -> Pack4d { return Pack4d{ 0.0, 0.0, v.x, v.y }; }
    constexpr inline auto pack2(const linalg::Vector2<double>& v) -> Pack2d { return Pack2d{ v.x, v.y }; }
    constexpr inline auto pack(const linalg::Vector3<double>& v) -> Pack4d { return Pack4d{ v.x, v.y, v.z, 0.0 }; }
    constexpr inline auto pack(const linalg::Vector4<double>& v) -> Pack4d { return Pack4d{ v.x, v.y, v.z, v.w }; }
    auto spread_vec2(Pack2d &&pack) -> linalg::Vector2<double> { return linalg::Vector2<double>{ pack.x, pack.y }; }
    auto spread_vec2(Pack4d &&pack) -> std::pair<linalg::Vector2<double>, linalg::Vector2<double>> { return { linalg::Vector2<double>{ pack.x, pack.y }, linalg::Vector2<double>{ pack.z, pack.w } }; }
    auto spread_vec3(Pack4d &&pack) -> linalg::Vector3<double> { return linalg::Vector3<double>{ pack.x, pack.y, pack.z }; }
    auto spread_vec4(Pack4d &&pack) -> linalg::Vector4<double> { return linalg::Vector4<double>{ pack.x, pack.y, pack.z, pack.w }; }
}