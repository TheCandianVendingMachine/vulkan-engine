#include "linalg/vector_ops.h"

#include <cmath>
#include <cstring>

namespace linalg {
    namespace blas1 {
        namespace detail {
            auto axpy4(float* out, const float a, const float* x, const float* y) -> float* {
                out[0] = a * x[0] + y[0];
                out[1] = a * x[1] + y[1];
                out[2] = a * x[2] + y[2];
                out[3] = a * x[3] + y[3];
                return out;
            }

            auto scale4(float* out, const float a, const float* x) -> float* {
                out[0] = a * x[0];
                out[1] = a * x[1];
                out[2] = a * x[2];
                out[3] = a * x[3];
                return out;
            }
        } // namespace detail

        auto axpy(const float a, const Vector4<float> x, const Vector4<float> y) -> Vector4<float> {
            auto out = Vector4<float>::zero();
            detail::axpy4(out.elements, a, x.elements, y.elements);
            return out;
        }

        auto scale(const float a, const Vector4<float> x) -> Vector4<float> {
            auto out = Vector4<float>::zero();
            detail::scale4(out.elements, a, x.elements);
            return out;
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
        namespace detail {
            auto axpy4(double* out, const double a, const double* x, const double* y) -> double* {
                out[0] = a * x[0] + y[0];
                out[1] = a * x[1] + y[1];
                out[2] = a * x[2] + y[2];
                out[3] = a * x[3] + y[3];
                return out;
            }

            auto scale4(double* out, const double a, const double* x) -> double* {
                out[0] = a * x[0];
                out[1] = a * x[1];
                out[2] = a * x[2];
                out[3] = a * x[3];
                return out;
            }
        } // namespace detail

        auto axpy(const double a, const Vector4<double> x, const Vector4<double> y) -> Vector4<double> {
            auto out = Vector4<double>::zero();
            detail::axpy4(out.elements, a, x.elements, y.elements);
            return out;
        }

        auto scale(const double a, const Vector4<double> x) -> Vector4<double> {
            auto out = Vector4<double>::zero();
            detail::scale4(out.elements, a, x.elements);
            return out;
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
