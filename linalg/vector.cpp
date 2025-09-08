#include "linalg/vector_ops.h"
#include <cmath>
#include <cstring>

namespace linalg {
    namespace blas1{
        auto axpy(float a, const Vector2<float>& x, Vector2<float>& y) -> Vector2<float> {
            return Vector2<float>{
                a * x.x + y.x,
                a * x.y + y.y
            };
        }

        auto axpy2(float a[2], const Vector2<float> x[2], Vector2<float> y[2]) -> std::pair<Vector2<float>, Vector2<float>> {
            return std::make_pair(
                axpy(a[0], x[0], y[0]),
                axpy(a[1], x[1], y[1])
            );
        }

        auto scale(float a, const Vector2<float>& x) -> Vector2<float> {
            return Vector2<float>{
                a * x.x,
                a * x.y
            };
        }

        auto scale2(float a[2], const Vector2<float> x[2]) -> std::pair<Vector2<float>, Vector2<float>> {
            return std::make_pair(
                scale(a[0], x[0]),
                scale(a[1], x[1])
            );
        }

        auto copy(Vector2<float>& a, const Vector2<float>& b) -> void {
            std::memcpy(&a.x, &b.x, sizeof(a.elements));
        }

        auto swap(Vector2<float>& a, Vector2<float>& b) -> void {
            std::swap(a.x, b.x);
            std::swap(a.y, b.y);
        }

        auto dot(Vector2<float>& a, const Vector2<float>& b) -> float {
            return a.x * b.x + a.y * b.y;
        }

        auto component_sum(Vector2<float>& x) -> float {
            return std::abs(x.x) + std::abs(x.y);
        }

        auto magnitude(Vector2<float>& x) -> float {
            return std::sqrtf(x.x * x.x + x.y * x.y);
        }

        auto component_max(Vector2<float>& x) -> float {
            return std::fmaxf(std::abs(x.x), std::abs(x.y));
        }
    }
}


namespace linalg {
    namespace blas1{
        auto axpy(double a, const Vector2<double>& x, Vector2<double>& y) -> Vector2<double> {
            return Vector2<double>{
                a * x.x + y.x,
                a * x.y + y.y
            };
        }

        auto scale(double a, const Vector2<double>& x) -> Vector2<double> {
            return Vector2<double>{
                a * x.x,
                a * x.y
            };
        }

        auto copy(Vector2<double>& a, const Vector2<double>& b) -> void {
            std::memcpy(&a.x, &b.x, sizeof(a.elements));
        }

        auto swap(Vector2<double>& a, Vector2<double>& b) -> void {
            std::swap(a.x, b.x);
            std::swap(a.y, b.y);
        }

        auto dot(Vector2<double>& a, const Vector2<double>& b) -> double {
            return a.x * b.x + a.y * b.y;
        }

        auto component_sum(Vector2<double>& x) -> double {
            return std::abs(x.x) + std::abs(x.y);
        }

        auto magnitude(Vector2<double>& x) -> double {
            return std::sqrt(x.x * x.x + x.y * x.y);
        }

        auto component_max(Vector2<double>& x) -> double {
            return std::fmax(std::abs(x.x), std::abs(x.y));
        }
    }
}