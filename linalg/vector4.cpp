#include "linalg/vector_ops.h"
#include <cmath>
#include <cstring>

namespace linalg {
namespace blas1 {
auto axpy(const float a, const Vector4<float> x, const Vector4<float> y) -> Vector4<float> {
    return Vector4<float>{a * x.x + y.x, a * x.y + y.y, a * x.z + y.z, a * x.w + y.w};
}

auto scale(const float a, const Vector4<float> x) -> Vector4<float> {
    return Vector4<float>{a * x.x, a * x.y, a * x.z, a * x.w};
}

auto copy(Vector4<float>& a, const Vector4<float> b) -> void {
    std::memcpy(&a.x, &b.x, sizeof(a.elements));
}

auto swap(Vector4<float>& a, Vector4<float>& b) -> void {
    std::swap(a.x, b.x);
    std::swap(a.y, b.y);
    std::swap(a.z, b.z);
    std::swap(a.w, b.w);
}

auto dot(const Vector4<float> a, const Vector4<float> b) -> float {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

auto component_sum(const Vector4<float> x) -> float {
    return std::abs(x.x) + std::abs(x.y) + std::abs(x.z) + std::abs(x.w);
}

auto magnitude(const Vector4<float> x) -> float {
    return std::sqrtf(x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w);
}

auto component_max(const Vector4<float> x) -> float {
    return std::fmaxf(std::fmaxf(std::abs(x.x), std::abs(x.y)), std::fmaxf(std::abs(x.z), std::abs(x.w)));
}
} // namespace blas1
} // namespace linalg

namespace linalg {
namespace blas1 {
auto axpy(const double a, const Vector4<double> x, const Vector4<double> y) -> Vector4<double> {
    return Vector4<double>{a * x.x + y.x, a * x.y + y.y, a * x.z + y.z, a * x.w + y.w};
}

auto scale(const double a, const Vector4<double> x) -> Vector4<double> {
    return Vector4<double>{a * x.x, a * x.y, a * x.z, a * x.w};
}

auto copy(Vector4<double>& a, const Vector4<double> b) -> void {
    a = b;
}

auto swap(Vector4<double>& a, Vector4<double>& b) -> void {
    std::swap(a.x, b.x);
    std::swap(a.y, b.y);
    std::swap(a.z, b.z);
    std::swap(a.w, b.w);
}

auto dot(const Vector4<double> a, const Vector4<double> b) -> double {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

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