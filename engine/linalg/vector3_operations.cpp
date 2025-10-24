#include "engine/linalg/vector.h"
#include <Tracy/Tracy.hpp>

using namespace linalg;

/*
    Addition
*/
template <>
auto linalg::operator+ <float>(const Vector3<float>& lhs, const Vector3<float>& rhs) -> Vector3<float> {
    ZoneScoped;
    auto out = Vector3<float>::zero();
    ENGINE_NS::linalg::blas1::vector3::float32::axpy(out.elements, 1.f, lhs.elements, rhs.elements);
    return out;
}
template <>
auto linalg::operator+ <double>(const Vector3<double>& lhs, const Vector3<double>& rhs) -> Vector3<double> {
    ZoneScoped;
    auto out = Vector3<double>::zero();
    ENGINE_NS::linalg::blas1::vector3::float64::axpy(out.elements, 1.f, lhs.elements, rhs.elements);
    return out;
}

template <>
auto linalg::operator+= <float>(Vector3<float>& lhs, const Vector3<float>& rhs) -> Vector3<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector3::float32::axpy(lhs.elements, 1.f, lhs.elements, rhs.elements);
    return lhs;
}
template <>
auto linalg::operator+= <double>(Vector3<double>& lhs, const Vector3<double>& rhs) -> Vector3<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector3::float64::axpy(lhs.elements, 1.f, lhs.elements, rhs.elements);
    return lhs;
}

/*
    Subtraction
*/
template <>
auto linalg::operator- <float>(const Vector3<float>& lhs, const Vector3<float>& rhs) -> Vector3<float> {
    ZoneScoped;
    auto out = Vector3<float>::zero();
    ENGINE_NS::linalg::blas1::vector3::float32::axpy(out.elements, -1.f, rhs.elements, lhs.elements);
    return out;
}
template <>
auto linalg::operator- <double>(const Vector3<double>& lhs, const Vector3<double>& rhs) -> Vector3<double> {
    ZoneScoped;
    auto out = Vector3<double>::zero();
    ENGINE_NS::linalg::blas1::vector3::float64::axpy(out.elements, -1.f, rhs.elements, lhs.elements);
    return out;
}

template <>
auto linalg::operator-= <float>(Vector3<float>& lhs, const Vector3<float>& rhs) -> Vector3<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector3::float32::axpy(lhs.elements, -1.f, rhs.elements, lhs.elements);
    return lhs;
}
template <>
auto linalg::operator-= <double>(Vector3<double>& lhs, const Vector3<double>& rhs) -> Vector3<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector3::float64::axpy(lhs.elements, -1.f, rhs.elements, lhs.elements);
    return lhs;
}

/*
    Scalar Multiplication
*/
template <>
auto linalg::operator* <float>(const Vector3<float>& lhs, float rhs) -> Vector3<float> {
    ZoneScoped;
    auto out = Vector3<float>::zero();
    ENGINE_NS::linalg::blas1::vector3::float32::scale(out.elements, rhs, lhs.elements);
    return out;
}
template <>
auto linalg::operator* <double>(const Vector3<double>& lhs, double rhs) -> Vector3<double> {
    ZoneScoped;
    auto out = Vector3<double>::zero();
    ENGINE_NS::linalg::blas1::vector3::float64::scale(out.elements, rhs, lhs.elements);
    return out;
}

template <>
auto linalg::operator*= <float>(Vector3<float>& lhs, float rhs) -> Vector3<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector3::float32::scale(lhs.elements, rhs, lhs.elements);
    return lhs;
}
template <>
auto linalg::operator*= <double>(Vector3<double>& lhs, double rhs) -> Vector3<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector3::float64::scale(lhs.elements, rhs, lhs.elements);
    return lhs;
}

template <>
auto linalg::operator* <float>(float lhs, const Vector3<float>& rhs) -> Vector3<float> {
    ZoneScoped;
    auto out = Vector3<float>::zero();
    ENGINE_NS::linalg::blas1::vector3::float32::scale(out.elements, lhs, rhs.elements);
    return out;
}
template <>
auto linalg::operator* <double>(double lhs, const Vector3<double>& rhs) -> Vector3<double> {
    ZoneScoped;
    auto out = Vector3<double>::zero();
    ENGINE_NS::linalg::blas1::vector3::float64::scale(out.elements, lhs, rhs.elements);
    return out;
}

template <>
auto linalg::operator*= <float>(float lhs, Vector3<float>& rhs) -> Vector3<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector3::float32::scale(rhs.elements, lhs, rhs.elements);
    return rhs;
}
template <>
auto linalg::operator*= <double>(double lhs, Vector3<double>& rhs) -> Vector3<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector3::float64::scale(rhs.elements, lhs, rhs.elements);
    return rhs;
}

/*
    Scalar Division
*/
template <>
auto linalg::operator/ <float>(const Vector3<float>& lhs, float rhs) -> Vector3<float> {
    ZoneScoped;
    auto out = Vector3<float>::zero();
    ENGINE_NS::linalg::blas1::vector3::float32::scale(out.elements, 1.f / rhs, lhs.elements);
    return out;
}
template <>
auto linalg::operator/ <double>(const Vector3<double>& lhs, double rhs) -> Vector3<double> {
    ZoneScoped;
    auto out = Vector3<double>::zero();
    ENGINE_NS::linalg::blas1::vector3::float64::scale(out.elements, 1.f / rhs, lhs.elements);
    return out;
}

template <>
auto linalg::operator/= <float>(Vector3<float>& lhs, float rhs) -> Vector3<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector3::float32::scale(lhs.elements, 1.f / rhs, lhs.elements);
    return lhs;
}
template <>
auto linalg::operator/= <double>(Vector3<double>& lhs, double rhs) -> Vector3<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector3::float64::scale(lhs.elements, 1.f / rhs, lhs.elements);
    return lhs;
}

/*
    Dot
*/
template <>
auto linalg::dot<float>(const Vector3<float>& lhs, const Vector3<float>& rhs) -> float {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector3::float32::dot(lhs.elements, rhs.elements);
}
template <>
auto linalg::dot<double>(const Vector3<double>& lhs, const Vector3<double>& rhs) -> double {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector3::float64::dot(lhs.elements, rhs.elements);
}

/*
    Magnitude
*/
template <>
auto linalg::magnitude<float>(const Vector3<float>& a) -> float {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector3::float32::magnitude(a.elements);
}
template <>
auto linalg::magnitude<double>(const Vector3<double>& a) -> double {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector3::float64::magnitude(a.elements);
}

/*
    Magnitude sqr
*/
template <>
auto linalg::magnitude_sqr<float>(const Vector3<float>& a) -> float {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector3::float32::dot(a.elements, a.elements);
}
template <>
auto linalg::magnitude_sqr<double>(const Vector3<double>& a) -> double {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector3::float64::dot(a.elements, a.elements);
}

/*
    Projection
*/
template <>
auto linalg::project<float>(const Vector3<float>& a, const Vector3<float>& b) -> Vector3<float> {
    ZoneScoped;
    auto dot_top    = dot(a, b);
    auto dot_bottom = dot(b, b);
    if (dot_bottom == 0.f) {
        return Vector3<float>::zero();
    }
    return dot_top / dot_bottom * b;
}
template <>
auto linalg::project<double>(const Vector3<double>& a, const Vector3<double>& b) -> Vector3<double> {
    ZoneScoped;
    auto dot_top    = dot(a, b);
    auto dot_bottom = dot(b, b);
    if (dot_bottom == 0.f) {
        return Vector3<double>::zero();
    }
    return dot_top / dot_bottom * b;
}

/*
    Normalization
*/
template <>
auto linalg::normalize<float>(const Vector3<float>& a) -> Vector3<float> {
    ZoneScoped;
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.f) {
        return a / magnitude;
    }
    return Vector3<float>::zero();
}
template <>
auto linalg::normalize<double>(const Vector3<double>& a) -> Vector3<double> {
    ZoneScoped;
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.0) {
        return a / magnitude;
    }
    return Vector3<double>::zero();
}

template <>
auto linalg::normalize_inplace<float>(Vector3<float>& a) -> Vector3<float>& {
    ZoneScoped;
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.f) {
        a /= magnitude;
    }
    return a;
}
template <>
auto linalg::normalize_inplace<double>(Vector3<double>& a) -> Vector3<double>& {
    ZoneScoped;
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.0) {
        a /= magnitude;
    }
    return a;
}

/*
    Cross product
*/
template <>
auto linalg::cross<float>(const Vector3<float>& a, const Vector3<float>& b) -> Vector3<float> {
    ZoneScoped;
    return Vector3<float>{
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x,
    };
}
template <>
auto linalg::cross<double>(const Vector3<double>& a, const Vector3<double>& b) -> Vector3<double> {
    ZoneScoped;
    return Vector3<double>{
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x,
    };
}
