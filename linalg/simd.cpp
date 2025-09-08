#include "linalg/simd.h"

namespace simd {
    using namespace linalg;
    auto pack1(float v) -> Pack4f { return Pack4f{ v, 0.f, 0.f, 0.f }; };
    auto pack2(float v) -> Pack2f { return Pack2f{ v, v }; };
    auto pack2_lower(float v) -> Pack4f { return Pack4f{ v, v, 0.f, 0.f }; };
    auto pack2_upper(float v) -> Pack4f { return Pack4f{ 0.f, 0.f, v, v }; };
    auto pack3_lower(float v) -> Pack4f { return Pack4f{ v, v, v, 0.f }; };
    auto pack3_upper(float v) -> Pack4f { return Pack4f{ 0.f, v, v, v }; };
    auto pack4(float v) -> Pack4f { return Pack4f{ v, v, v, v }; };
    auto pack_even(float v) -> Pack4f { return Pack4f{ v, 0.f, v, 0.f }; }
    auto pack_odd(float v) -> Pack4f { return Pack4f { 0.f, v, 0.f, v }; };
    auto pack_lower(const Vector2<float>& v) -> Pack4f { return Pack4f{ v.x, v.y, 0.f, 0.f }; };
    auto pack_upper(const Vector2<float>& v) -> Pack4f { return Pack4f{ 0.f, 0.f, v.x, v.y }; };
    auto pack2(const Vector2<float>& v) -> Pack2f { return Pack2f{ v.x, v.y }; };
    auto pack(const Vector3<float>& v) -> Pack4f { return Pack4f{ v.x, v.y, v.z, 0.f }; };
    auto pack(const Vector4<float>& v) -> Pack4f { return Pack4f{ v.x, v.y, v.z, v.w }; };
    auto spread_vec2(Pack2f &&pack) -> Vector2<float> { return Vector2<float>{ pack.x, pack.y }; };
    auto spread_vec2(Pack4f &&pack) -> std::pair<Vector2<float>, Vector2<float>> { return { Vector2<float>{ pack.x, pack.y }, Vector2<float>{ pack.z, pack.w } }; };
    auto spread_vec3(Pack4f &&pack) -> Vector3<float> { return Vector3<float>{ pack.x, pack.y, pack.z }; };
    auto spread_vec4(Pack4f &&pack) -> Vector4<float> { return Vector4<float>{ pack.x, pack.y, pack.z, pack.w }; };

    auto pack1(double v) -> Pack4d { return Pack4d{ v, 0.0, 0.0, 0.0 }; };
    auto pack2(double v) -> Pack2d { return Pack2d{ v, v }; };
    auto pack2_lower(double v) -> Pack4d { return Pack4d{ v, v, 0.0, 0.0 }; };
    auto pack2_upper(double v) -> Pack4d { return Pack4d{ 0.0, 0.0, v, v }; };
    auto pack3_lower(double v) -> Pack4d { return Pack4d{ v, v, v, 0.0 }; };
    auto pack3_upper(double v) -> Pack4d { return Pack4d{ 0.0, v, v, v }; };
    auto pack4(double v) -> Pack4d { return Pack4d{ v, v, v, v }; };
    auto pack_even(double v) -> Pack4d { return Pack4d{ v, 0.0, v, 0.0 }; }
    auto pack_odd(double v) -> Pack4d { return Pack4d { 0.0, v, 0.0, v }; };
    auto pack_lower(const Vector2<double>& v) -> Pack4d { return Pack4d{ v.x, v.y, 0.0, 0.0 }; };
    auto pack_upper(const Vector2<double>& v) -> Pack4d { return Pack4d{ 0.0, 0.0, v.x, v.y }; };
    auto pack2(const Vector2<double>& v) -> Pack2d { return Pack2d{ v.x, v.y }; };
    auto pack(const Vector3<double>& v) -> Pack4d { return Pack4d{ v.x, v.y, v.z, 0.0 }; };
    auto pack(const Vector4<double>& v) -> Pack4d { return Pack4d{ v.x, v.y, v.z, v.w }; };
    auto spread_vec2(Pack2d &&pack) -> Vector2<double> { return Vector2<double>{ pack.x, pack.y }; };
    auto spread_vec2(Pack4d &&pack) -> std::pair<Vector2<double>, Vector2<double>> { return { Vector2<double>{ pack.x, pack.y }, Vector2<double>{ pack.z, pack.w } }; };
    auto spread_vec3(Pack4d &&pack) -> Vector3<double> { return Vector3<double>{ pack.x, pack.y, pack.z }; };
    auto spread_vec4(Pack4d &&pack) -> Vector4<double> { return Vector4<double>{ pack.x, pack.y, pack.z, pack.w }; };
}