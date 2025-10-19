#include "linalg/vector_ops.h"
#include <cmath>
#include <cstring>

namespace linalg {
namespace blas1 {
auto axpy(const float a, const Vector3<float> x, const Vector3<float> y) -> Vector3<float> {
    return Vector3<float>{a * x.x + y.x, a * x.y + y.y, a * x.z + y.z};
}

auto scale(const float a, const Vector3<float> x) -> Vector3<float> {
    return Vector3<float>{a * x.x, a * x.y, a * x.z};
}

auto copy(Vector3<float>& a, const Vector3<float> b) -> void {
    std::memcpy(&a.x, &b.x, sizeof(a.elements));
}

auto swap(Vector3<float>& a, Vector3<float>& b) -> void {
    std::swap(a.x, b.x);
    std::swap(a.y, b.y);
    std::swap(a.z, b.z);
}

auto dot(const Vector3<float> a, const Vector3<float> b) -> float {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

auto component_sum(const Vector3<float> x) -> float {
    return std::abs(x.x) + std::abs(x.y) + std::abs(x.z);
}

auto magnitude(const Vector3<float> x) -> float {
    return std::sqrtf(x.x * x.x + x.y * x.y + x.z * x.z);
}

auto component_max(const Vector3<float> x) -> float {
    return std::fmaxf(std::abs(x.x), std::fmaxf(std::abs(x.y), std::abs(x.z)));
}
} // namespace blas1
} // namespace linalg

namespace linalg {
namespace blas1 {
auto axpy(const double a, const Vector3<double> x, const Vector3<double> y) -> Vector3<double> {
    return Vector3<double>{a * x.x + y.x, a * x.y + y.y, a * x.z + y.z};
}

auto scale(const double a, const Vector3<double> x) -> Vector3<double> {
    return Vector3<double>{a * x.x, a * x.y, a * x.z};
}

auto copy(Vector3<double>& a, const Vector3<double> b) -> void {
    a = b;
}

auto swap(Vector3<double>& a, Vector3<double>& b) -> void {
    std::swap(a.x, b.x);
    std::swap(a.y, b.y);
    std::swap(a.z, b.z);
}

auto dot(const Vector3<double> a, const Vector3<double> b) -> double {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

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