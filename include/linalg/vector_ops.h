#pragma once

#include "linalg/vector.h"

namespace linalg {
    namespace blas1 {
        auto axpy(float a, const Vector2<float>& x, Vector2<float>& y) -> Vector2<float>;
        auto axpy2(float a[2], const Vector2<float> x[2], Vector2<float> y[2]) -> std::pair<Vector2<float>, Vector2<float>>;
        auto scale(float a, const Vector2<float>& x) -> Vector2<float>;
        auto scale2(float a[2], const Vector2<float> x[2]) -> std::pair<Vector2<float>, Vector2<float>>;
        auto copy(Vector2<float>& a, const Vector2<float>& b) -> void;
        auto swap(Vector2<float>& a, Vector2<float>& b) -> void;
        auto dot(Vector2<float>& a, const Vector2<float>& b) -> float;
        auto component_sum(Vector2<float>& x) -> float;
        auto magnitude(Vector2<float>& x) -> float;
        auto component_max(Vector2<float>& x) -> float;

        auto axpy(double a, const Vector2<double>& x, Vector2<double>& y) -> Vector2<double>;
        auto axpy2(double a[2], const Vector2<double> x[2], Vector2<double> y[2]) -> std::pair<Vector2<double>, Vector2<double>>;
        auto scale(double a, const Vector2<double>& x) -> Vector2<double>;
        auto scale2(double a[2], const Vector2<double> x[2]) -> std::pair<Vector2<double>, Vector2<double>>;
        auto copy(Vector2<double>& a, const Vector2<double>& b) -> void;
        auto swap(Vector2<double>& a, Vector2<double>& b) -> void;
        auto dot(Vector2<double>& a, const Vector2<double>& b) -> double;
        auto component_sum(Vector2<double>& x) -> double;
        auto magnitude(Vector2<double>& x) -> double;
        auto component_max(Vector2<double>& x) -> double;
    }
}
