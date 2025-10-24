#include "engine/linalg/vector.h"
#include <tracy/Tracy.hpp>

using namespace linalg;

/*
    Addition
*/
template <>
auto linalg::operator+ <float>(const Vector2<float>& lhs, const Vector2<float>& rhs) -> Vector2<float> {
    ZoneScoped;
    auto out = Vector2<float>::zero();
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(out.elements, 1.f, lhs.elements, rhs.elements);
    return out;
}
template <>
auto linalg::operator+ <double>(const Vector2<double>& lhs, const Vector2<double>& rhs) -> Vector2<double> {
    ZoneScoped;
    auto out = Vector2<double>::zero();
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(out.elements, 1.f, lhs.elements, rhs.elements);
    return out;
}

template <>
auto linalg::operator+= <float>(Vector2<float>& lhs, const Vector2<float>& rhs) -> Vector2<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(lhs.elements, 1.f, lhs.elements, rhs.elements);
    return lhs;
}
template <>
auto linalg::operator+= <double>(Vector2<double>& lhs, const Vector2<double>& rhs) -> Vector2<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(lhs.elements, 1.f, lhs.elements, rhs.elements);
    return lhs;
}

/*
    Subtraction
*/
template <>
auto linalg::operator- <float>(const Vector2<float>& lhs, const Vector2<float>& rhs) -> Vector2<float> {
    ZoneScoped;
    auto out = Vector2<float>::zero();
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(out.elements, -1.f, rhs.elements, lhs.elements);
    return out;
}
template <>
auto linalg::operator- <double>(const Vector2<double>& lhs, const Vector2<double>& rhs) -> Vector2<double> {
    ZoneScoped;
    auto out = Vector2<double>::zero();
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(out.elements, -1.f, rhs.elements, lhs.elements);
    return out;
}

template <>
auto linalg::operator-= <float>(Vector2<float>& lhs, const Vector2<float>& rhs) -> Vector2<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(lhs.elements, -1.f, rhs.elements, lhs.elements);
    return lhs;
}
template <>
auto linalg::operator-= <double>(Vector2<double>& lhs, const Vector2<double>& rhs) -> Vector2<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(lhs.elements, -1.f, rhs.elements, lhs.elements);
    return lhs;
}

/*
    Scalar Multiplication
*/
template <>
auto linalg::operator* <float>(const Vector2<float>& lhs, float rhs) -> Vector2<float> {
    ZoneScoped;
    auto out = Vector2<float>::zero();
    ENGINE_NS::linalg::blas1::vector2::float32::scale(out.elements, rhs, lhs.elements);
    return out;
}
template <>
auto linalg::operator* <double>(const Vector2<double>& lhs, double rhs) -> Vector2<double> {
    ZoneScoped;
    auto out = Vector2<double>::zero();
    ENGINE_NS::linalg::blas1::vector2::float64::scale(out.elements, rhs, lhs.elements);
    return out;
}

template <>
auto linalg::operator*= <float>(Vector2<float>& lhs, float rhs) -> Vector2<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float32::scale(lhs.elements, rhs, lhs.elements);
    return lhs;
}
template <>
auto linalg::operator*= <double>(Vector2<double>& lhs, double rhs) -> Vector2<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float64::scale(lhs.elements, rhs, lhs.elements);
    return lhs;
}

template <>
auto linalg::operator* <float>(float lhs, const Vector2<float>& rhs) -> Vector2<float> {
    ZoneScoped;
    auto out = Vector2<float>::zero();
    ENGINE_NS::linalg::blas1::vector2::float32::scale(out.elements, lhs, rhs.elements);
    return out;
}
template <>
auto linalg::operator* <double>(double lhs, const Vector2<double>& rhs) -> Vector2<double> {
    ZoneScoped;
    auto out = Vector2<double>::zero();
    ENGINE_NS::linalg::blas1::vector2::float64::scale(out.elements, lhs, rhs.elements);
    return out;
}

template <>
auto linalg::operator*= <float>(float lhs, Vector2<float>& rhs) -> Vector2<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float32::scale(rhs.elements, lhs, rhs.elements);
    return rhs;
}
template <>
auto linalg::operator*= <double>(double lhs, Vector2<double>& rhs) -> Vector2<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float64::scale(rhs.elements, lhs, rhs.elements);
    return rhs;
}

/*
    Scalar Division
*/
template <>
auto linalg::operator/ <float>(const Vector2<float>& lhs, float rhs) -> Vector2<float> {
    ZoneScoped;
    auto out = Vector2<float>::zero();
    ENGINE_NS::linalg::blas1::vector2::float32::scale(out.elements, 1.f / rhs, lhs.elements);
    return out;
}
template <>
auto linalg::operator/ <double>(const Vector2<double>& lhs, double rhs) -> Vector2<double> {
    ZoneScoped;
    auto out = Vector2<double>::zero();
    ENGINE_NS::linalg::blas1::vector2::float64::scale(out.elements, 1.f / rhs, lhs.elements);
    return out;
}

template <>
auto linalg::operator/= <float>(Vector2<float>& lhs, float rhs) -> Vector2<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float32::scale(lhs.elements, 1.f / rhs, lhs.elements);
    return lhs;
}
template <>
auto linalg::operator/= <double>(Vector2<double>& lhs, double rhs) -> Vector2<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float64::scale(lhs.elements, 1.f / rhs, lhs.elements);
    return lhs;
}

/*
    Dot
*/
template <>
auto linalg::dot<float>(const Vector2<float>& lhs, const Vector2<float>& rhs) -> float {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector2::float32::dot(lhs.elements, rhs.elements);
}
template <>
auto linalg::dot<double>(const Vector2<double>& lhs, const Vector2<double>& rhs) -> double {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector2::float64::dot(lhs.elements, rhs.elements);
}

/*
    Magnitude
*/
template <>
auto linalg::magnitude<float>(const Vector2<float>& a) -> float {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector2::float32::magnitude(a.elements);
}
template <>
auto linalg::magnitude<double>(const Vector2<double>& a) -> double {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector2::float64::magnitude(a.elements);
}

/*
    Magnitude sqr
*/
template <>
auto linalg::magnitude_sqr<float>(const Vector2<float>& a) -> float {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector2::float32::dot(a.elements, a.elements);
}
template <>
auto linalg::magnitude_sqr<double>(const Vector2<double>& a) -> double {
    ZoneScoped;
    return ENGINE_NS::linalg::blas1::vector2::float64::dot(a.elements, a.elements);
}

/*
    Projection
*/
template <>
auto linalg::project<float>(const Vector2<float>& a, const Vector2<float>& b) -> Vector2<float> {
    ZoneScoped;
    auto dot_top    = dot(a, b);
    auto dot_bottom = dot(b, b);
    if (dot_bottom == 0.f) {
        return Vector2<float>::zero();
    }
    return dot_top / dot_bottom * b;
}
template <>
auto linalg::project<double>(const Vector2<double>& a, const Vector2<double>& b) -> Vector2<double> {
    ZoneScoped;
    auto dot_top    = dot(a, b);
    auto dot_bottom = dot(b, b);
    if (dot_bottom == 0.f) {
        return Vector2<double>::zero();
    }
    return dot_top / dot_bottom * b;
}

/*
    Normalization
*/
template <>
auto linalg::normalize<float>(const Vector2<float>& a) -> Vector2<float> {
    ZoneScoped;
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.f) {
        return a / magnitude;
    }
    return Vector2<float>::zero();
}
template <>
auto linalg::normalize<double>(const Vector2<double>& a) -> Vector2<double> {
    ZoneScoped;
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.0) {
        return a / magnitude;
    }
    return Vector2<double>::zero();
}

template <>
auto linalg::normalize_inplace<float>(Vector2<float>& a) -> Vector2<float>& {
    ZoneScoped;
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.f) {
        a /= magnitude;
    }
    return a;
}
template <>
auto linalg::normalize_inplace<double>(Vector2<double>& a) -> Vector2<double>& {
    ZoneScoped;
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.0) {
        a /= magnitude;
    }
    return a;
}
