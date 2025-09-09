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
