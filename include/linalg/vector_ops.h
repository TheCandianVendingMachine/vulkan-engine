#pragma once

#include "linalg/vector.h"

namespace linalg {
    namespace blas1 {
        auto axpy(const float a, const Vector2<float> x, const Vector2<float> y) -> Vector2<float>;
        auto scale(const float a, const Vector2<float> x) -> Vector2<float>;
        auto copy(Vector2<float>& a, const Vector2<float> b) -> void;
        auto swap(Vector2<float>& a, Vector2<float>& b) -> void;
        auto dot(const Vector2<float> a, const Vector2<float> b) -> float;
        auto component_sum(const Vector2<float> x) -> float;
        auto magnitude(const Vector2<float> x) -> float;
        auto component_max(const Vector2<float> x) -> float;

        auto axpy(const double a, const Vector2<double> x, const Vector2<double> y) -> Vector2<double>;
        auto scale(const double a, const Vector2<double> x) -> Vector2<double>;
        auto copy(Vector2<double>& a, const Vector2<double> b) -> void;
        auto swap(Vector2<double>& a, Vector2<double>& b) -> void;
        auto dot(const Vector2<double> a, const Vector2<double> b) -> double;
        auto component_sum(const Vector2<double> x) -> double;
        auto magnitude(const Vector2<double> x) -> double;
        auto component_max(const Vector2<double> x) -> double;
    }
}

namespace linalg {
    namespace blas1 {
        auto axpy(const float a, const Vector3<float> x, const Vector3<float> y) -> Vector3<float>;
        auto scale(const float a, const Vector3<float> x) -> Vector3<float>;
        auto copy(Vector3<float>& a, const Vector3<float> b) -> void;
        auto swap(Vector3<float>& a, Vector3<float>& b) -> void;
        auto dot(const Vector3<float> a, const Vector3<float> b) -> float;
        auto component_sum(const Vector3<float> x) -> float;
        auto magnitude(const Vector3<float> x) -> float;
        auto component_max(const Vector3<float> x) -> float;

        auto axpy(const double a, const Vector3<double> x, const Vector3<double> y) -> Vector3<double>;
        auto scale(const double a, const Vector3<double> x) -> Vector3<double>;
        auto copy(Vector3<double>& a, const Vector3<double> b) -> void;
        auto swap(Vector3<double>& a, Vector3<double>& b) -> void;
        auto dot(const Vector3<double> a, const Vector3<double> b) -> double;
        auto component_sum(const Vector3<double> x) -> double;
        auto magnitude(const Vector3<double> x) -> double;
        auto component_max(const Vector3<double> x) -> double;
    }
}

namespace linalg {
    namespace blas1 {
        auto axpy(const float a, const Vector4<float> x, const Vector4<float> y) -> Vector4<float>;
        auto scale(const float a, const Vector4<float> x) -> Vector4<float>;
        auto copy(Vector4<float>& a, const Vector4<float> b) -> void;
        auto swap(Vector4<float>& a, Vector4<float>& b) -> void;
        auto dot(const Vector4<float> a, const Vector4<float> b) -> float;
        auto component_sum(const Vector4<float> x) -> float;
        auto magnitude(const Vector4<float> x) -> float;
        auto component_max(const Vector4<float> x) -> float;

        auto axpy(const double a, const Vector4<double> x, const Vector4<double> y) -> Vector4<double>;
        auto scale(const double a, const Vector4<double> x) -> Vector4<double>;
        auto copy(Vector4<double>& a, const Vector4<double> b) -> void;
        auto swap(Vector4<double>& a, Vector4<double>& b) -> void;
        auto dot(const Vector4<double> a, const Vector4<double> b) -> double;
        auto component_sum(const Vector4<double> x) -> double;
        auto magnitude(const Vector4<double> x) -> double;
        auto component_max(const Vector4<double> x) -> double;
    }
}
