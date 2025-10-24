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
    } // namespace blas1
} // namespace linalg

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
    } // namespace blas1
} // namespace linalg

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
    } // namespace blas1
} // namespace linalg

extern "C"
{
    __declspec(dllexport) float* saxpy2(float* out, const float a, const float* x, const float* y);
    __declspec(dllexport) float* sscale2(float* out, const float a, const float* x);
    __declspec(dllexport) void scopy2(float* a, const float* b);
    __declspec(dllexport) void sswap2(float* a, float* b);
    __declspec(dllexport) float sdot2(const float* a, const float* b);
    __declspec(dllexport) float scomponentsum2(const float* x);
    __declspec(dllexport) float smagnitude2(const float* x);
    __declspec(dllexport) float scomponentmax2(const float* x);

    __declspec(dllexport) double* daxpy2(double* out, const double a, const double* x, const double* y);
    __declspec(dllexport) double* dscale2(double* out, const double a, const double* x);
    __declspec(dllexport) void dcopy2(double* a, const double* b);
    __declspec(dllexport) void dswap2(double* a, double* b);
    __declspec(dllexport) double ddot2(const double* a, const double* b);
    __declspec(dllexport) double dcomponentsum2(const double* x);
    __declspec(dllexport) double dmagnitude2(const double* x);
    __declspec(dllexport) double dcomponentmax2(const double* x);
}

extern "C"
{
    __declspec(dllexport) float* saxpy3(float* out, const float a, const float* x, const float* y);
    __declspec(dllexport) float* sscale3(float* out, const float a, const float* x);
    __declspec(dllexport) void scopy3(float* a, const float* b);
    __declspec(dllexport) void sswap3(float* a, float* b);
    __declspec(dllexport) float sdot3(const float* a, const float* b);
    __declspec(dllexport) float scomponentsum3(const float* x);
    __declspec(dllexport) float smagnitude3(const float* x);
    __declspec(dllexport) float scomponentmax3(const float* x);

    __declspec(dllexport) double* daxpy3(double* out, const double a, const double* x, const double* y);
    __declspec(dllexport) double* dscale3(double* out, const double a, const double* x);
    __declspec(dllexport) void dcopy3(double* a, const double* b);
    __declspec(dllexport) void dswap3(double* a, double* b);
    __declspec(dllexport) double ddot3(const double* a, const double* b);
    __declspec(dllexport) double dcomponentsum3(const double* x);
    __declspec(dllexport) double dmagnitude3(const double* x);
    __declspec(dllexport) double dcomponentmax3(const double* x);
}

extern "C"
{
    __declspec(dllexport) float* saxpy4(float* out, const float a, const float* x, const float* y);
    __declspec(dllexport) float* sscale4(float* out, const float a, const float* x);
    __declspec(dllexport) void scopy4(float* a, const float* b);
    __declspec(dllexport) void sswap4(float* a, float* b);
    __declspec(dllexport) float sdot4(const float* a, const float* b);
    __declspec(dllexport) float scomponentsum4(const float* x);
    __declspec(dllexport) float smagnitude4(const float* x);
    __declspec(dllexport) float scomponentmax4(const float* x);

    __declspec(dllexport) double* daxpy4(double* out, const double a, const double* x, const double* y);
    __declspec(dllexport) double* dscale4(double* out, const double a, const double* x);
    __declspec(dllexport) void dcopy4(double* a, const double* b);
    __declspec(dllexport) void dswap4(double* a, double* b);
    __declspec(dllexport) double ddot4(const double* a, const double* b);
    __declspec(dllexport) double dcomponentsum4(const double* x);
    __declspec(dllexport) double dmagnitude4(const double* x);
    __declspec(dllexport) double dcomponentmax4(const double* x);
}
