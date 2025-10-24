#include "engine/linalg/vector.h"

using namespace linalg;

/*
    Addition
*/
template <>
auto linalg::operator+ <float>(const Vector4<float>& lhs, const Vector4<float>& rhs) -> Vector4<float> {
    auto out = Vector4<float>::zero();
    ENGINE_NS::linalg::blas1::vector4::float32::axpy(out.elements, 1.f, lhs.elements, rhs.elements);
    return out;
}
template <>
auto linalg::operator+ <double>(const Vector4<double>& lhs, const Vector4<double>& rhs) -> Vector4<double> {
    auto out = Vector4<double>::zero();
    ENGINE_NS::linalg::blas1::vector4::float64::axpy(out.elements, 1.f, lhs.elements, rhs.elements);
    return out;
}

template <>
auto linalg::operator+= <float>(Vector4<float>& lhs, const Vector4<float>& rhs) -> Vector4<float>& {
    ENGINE_NS::linalg::blas1::vector4::float32::axpy(lhs.elements, 1.f, lhs.elements, rhs.elements);
    return lhs;
}
template <>
auto linalg::operator+= <double>(Vector4<double>& lhs, const Vector4<double>& rhs) -> Vector4<double>& {
    ENGINE_NS::linalg::blas1::vector4::float64::axpy(lhs.elements, 1.f, lhs.elements, rhs.elements);
    return lhs;
}

/*
    Subtraction
*/
template <>
auto linalg::operator- <float>(const Vector4<float>& lhs, const Vector4<float>& rhs) -> Vector4<float> {
    auto out = Vector4<float>::zero();
    ENGINE_NS::linalg::blas1::vector4::float32::axpy(out.elements, -1.f, rhs.elements, lhs.elements);
    return out;
}
template <>
auto linalg::operator- <double>(const Vector4<double>& lhs, const Vector4<double>& rhs) -> Vector4<double> {
    auto out = Vector4<double>::zero();
    ENGINE_NS::linalg::blas1::vector4::float64::axpy(out.elements, -1.f, rhs.elements, lhs.elements);
    return out;
}

template <>
auto linalg::operator-= <float>(Vector4<float>& lhs, const Vector4<float>& rhs) -> Vector4<float>& {
    ENGINE_NS::linalg::blas1::vector4::float32::axpy(lhs.elements, -1.f, rhs.elements, lhs.elements);
    return lhs;
}
template <>
auto linalg::operator-= <double>(Vector4<double>& lhs, const Vector4<double>& rhs) -> Vector4<double>& {
    ENGINE_NS::linalg::blas1::vector4::float64::axpy(lhs.elements, -1.f, rhs.elements, lhs.elements);
    return lhs;
}

/*
    Scalar Multiplication
*/
template <>
auto linalg::operator* <float>(const Vector4<float>& lhs, float rhs) -> Vector4<float> {
    auto out = Vector4<float>::zero();
    ENGINE_NS::linalg::blas1::vector4::float32::scale(out.elements, rhs, lhs.elements);
    return out;
}
template <>
auto linalg::operator* <double>(const Vector4<double>& lhs, double rhs) -> Vector4<double> {
    auto out = Vector4<double>::zero();
    ENGINE_NS::linalg::blas1::vector4::float64::scale(out.elements, rhs, lhs.elements);
    return out;
}

template <>
auto linalg::operator*= <float>(Vector4<float>& lhs, float rhs) -> Vector4<float>& {
    ENGINE_NS::linalg::blas1::vector4::float32::scale(lhs.elements, rhs, lhs.elements);
    return lhs;
}
template <>
auto linalg::operator*= <double>(Vector4<double>& lhs, double rhs) -> Vector4<double>& {
    ENGINE_NS::linalg::blas1::vector4::float64::scale(lhs.elements, rhs, lhs.elements);
    return lhs;
}

template <>
auto linalg::operator* <float>(float lhs, const Vector4<float>& rhs) -> Vector4<float> {
    auto out = Vector4<float>::zero();
    ENGINE_NS::linalg::blas1::vector4::float32::scale(out.elements, lhs, rhs.elements);
    return out;
}
template <>
auto linalg::operator* <double>(double lhs, const Vector4<double>& rhs) -> Vector4<double> {
    auto out = Vector4<double>::zero();
    ENGINE_NS::linalg::blas1::vector4::float64::scale(out.elements, lhs, rhs.elements);
    return out;
}

template <>
auto linalg::operator*= <float>(float lhs, Vector4<float>& rhs) -> Vector4<float>& {
    ENGINE_NS::linalg::blas1::vector4::float32::scale(rhs.elements, lhs, rhs.elements);
    return rhs;
}
template <>
auto linalg::operator*= <double>(double lhs, Vector4<double>& rhs) -> Vector4<double>& {
    ENGINE_NS::linalg::blas1::vector4::float64::scale(rhs.elements, lhs, rhs.elements);
    return rhs;
}

/*
    Scalar Division
*/
template <>
auto linalg::operator/ <float>(const Vector4<float>& lhs, float rhs) -> Vector4<float> {
    auto out = Vector4<float>::zero();
    ENGINE_NS::linalg::blas1::vector4::float32::scale(out.elements, 1.f / rhs, lhs.elements);
    return out;
}
template <>
auto linalg::operator/ <double>(const Vector4<double>& lhs, double rhs) -> Vector4<double> {
    auto out = Vector4<double>::zero();
    ENGINE_NS::linalg::blas1::vector4::float64::scale(out.elements, 1.f / rhs, lhs.elements);
    return out;
}

template <>
auto linalg::operator/= <float>(Vector4<float>& lhs, float rhs) -> Vector4<float>& {
    ENGINE_NS::linalg::blas1::vector4::float32::scale(lhs.elements, 1.f / rhs, lhs.elements);
    return lhs;
}
template <>
auto linalg::operator/= <double>(Vector4<double>& lhs, double rhs) -> Vector4<double>& {
    ENGINE_NS::linalg::blas1::vector4::float64::scale(lhs.elements, 1.f / rhs, lhs.elements);
    return lhs;
}

/*
    Dot
*/
template <>
auto linalg::dot<float>(const Vector4<float>& lhs, const Vector4<float>& rhs) -> float {
    return ENGINE_NS::linalg::blas1::vector4::float32::dot(lhs.elements, rhs.elements);
}
template <>
auto linalg::dot<double>(const Vector4<double>& lhs, const Vector4<double>& rhs) -> double {
    return ENGINE_NS::linalg::blas1::vector4::float64::dot(lhs.elements, rhs.elements);
}

/*
    Magnitude
*/
template <>
auto linalg::magnitude<float>(const Vector4<float>& a) -> float {
    return ENGINE_NS::linalg::blas1::vector4::float32::magnitude(a.elements);
}
template <>
auto linalg::magnitude<double>(const Vector4<double>& a) -> double {
    return ENGINE_NS::linalg::blas1::vector4::float64::magnitude(a.elements);
}

/*
    Magnitude sqr
*/
template <>
auto linalg::magnitude_sqr<float>(const Vector4<float>& a) -> float {
    return ENGINE_NS::linalg::blas1::vector4::float32::dot(a.elements, a.elements);
}
template <>
auto linalg::magnitude_sqr<double>(const Vector4<double>& a) -> double {
    return ENGINE_NS::linalg::blas1::vector4::float64::dot(a.elements, a.elements);
}

/*
    Projection
*/
template <>
auto linalg::project<float>(const Vector4<float>& a, const Vector4<float>& b) -> Vector4<float> {
    auto dot_top    = dot(a, b);
    auto dot_bottom = dot(b, b);
    if (dot_bottom == 0.f) {
        return Vector4<float>::zero();
    }
    return dot_top / dot_bottom * b;
}
template <>
auto linalg::project<double>(const Vector4<double>& a, const Vector4<double>& b) -> Vector4<double> {
    auto dot_top    = dot(a, b);
    auto dot_bottom = dot(b, b);
    if (dot_bottom == 0.f) {
        return Vector4<double>::zero();
    }
    return dot_top / dot_bottom * b;
}

/*
    Normalization
*/
template <>
auto linalg::normalize<float>(const Vector4<float>& a) -> Vector4<float> {
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.f) {
        return a / magnitude;
    }
    return Vector4<float>::zero();
}
template <>
auto linalg::normalize<double>(const Vector4<double>& a) -> Vector4<double> {
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.0) {
        return a / magnitude;
    }
    return Vector4<double>::zero();
}

template <>
auto linalg::normalize_inplace<float>(Vector4<float>& a) -> Vector4<float>& {
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.f) {
        a /= magnitude;
    }
    return a;
}
template <>
auto linalg::normalize_inplace<double>(Vector4<double>& a) -> Vector4<double>& {
    auto magnitude = linalg::magnitude(a);
    if (magnitude > 0.0) {
        a /= magnitude;
    }
    return a;
}
