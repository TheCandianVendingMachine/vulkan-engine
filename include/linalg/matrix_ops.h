#pragma once
#include "linalg/matrix.h"
#include "linalg/vector.h"

namespace linalg {
    namespace blas2 {
        auto matrix_vector_product(const Matrix2<float>& A, const Vector2<float> x) -> Vector2<float>;
        auto solve_lower_triangular(const Matrix2<float>& L, const Vector2<float> b) -> Vector2<float>;
        auto solve_upper_triangular(const Matrix2<float>& U, const Vector2<float> b) -> Vector2<float>;
        auto solve(const Matrix2<float>& A, const Vector2<float> b) -> Vector2<float>;

        auto matrix_vector_product(const Matrix2<double>& A, const Vector2<double> x) -> Vector2<double>;
        auto solve_lower_triangular(const Matrix2<double>& L, const Vector2<double> b) -> Vector2<double>;
        auto solve_upper_triangular(const Matrix2<double>& U, const Vector2<double> b) -> Vector2<double>;
        auto solve(const Matrix2<double>& A, const Vector2<double> b) -> Vector2<double>;
    } // namespace blas2
} // namespace linalg

namespace linalg {
    namespace blas2 {
        auto matrix_vector_product(const Matrix3<float>& A, const Vector3<float> x) -> Vector3<float>;
        auto solve_lower_triangular(const Matrix3<float>& L, const Vector3<float> b) -> Vector3<float>;
        auto solve_upper_triangular(const Matrix3<float>& U, const Vector3<float> b) -> Vector3<float>;
        auto solve(const Matrix3<float>& A, const Vector3<float> b) -> Vector3<float>;

        auto matrix_vector_product(const Matrix3<double>& A, const Vector3<double> x) -> Vector3<double>;
        auto solve_lower_triangular(const Matrix3<double>& L, const Vector3<double> b) -> Vector3<double>;
        auto solve_upper_triangular(const Matrix3<double>& U, const Vector3<double> b) -> Vector3<double>;
        auto solve(const Matrix3<double>& A, const Vector3<double> b) -> Vector3<double>;
    } // namespace blas2
} // namespace linalg

namespace linalg {
    namespace blas2 {
        auto matrix_vector_product(const Matrix4<float>& A, const Vector4<float> x) -> Vector4<float>;
        auto solve_lower_triangular(const Matrix4<float>& L, const Vector4<float> b) -> Vector4<float>;
        auto solve_upper_triangular(const Matrix4<float>& U, const Vector4<float> b) -> Vector4<float>;
        auto solve(const Matrix4<float>& A, const Vector4<float> b) -> Vector4<float>;

        auto matrix_vector_product(const Matrix4<double>& A, const Vector4<double> x) -> Vector4<double>;
        auto solve_lower_triangular(const Matrix4<double>& L, const Vector4<double> b) -> Vector4<double>;
        auto solve_upper_triangular(const Matrix4<double>& U, const Vector4<double> b) -> Vector4<double>;
        auto solve(const Matrix4<double>& A, const Vector4<double> b) -> Vector4<double>;
    } // namespace blas2
} // namespace linalg

extern "C"
{
    __declspec(dllexport) float* smvprod2(float* out, float* A, const float* x);
    __declspec(dllexport) float* ssolve2l(float* out, float* L, float* b);
    __declspec(dllexport) float* ssolve2u(float* out, float* U, float* b);
    __declspec(dllexport) float* ssolve2(float* out, float* A, float* b);

    __declspec(dllexport) double* dmvprod2(double* out, double* A, const double* x);
    __declspec(dllexport) double* dsolve2l(double* out, double* L, double* b);
    __declspec(dllexport) double* dsolve2u(double* out, double* U, double* b);
    __declspec(dllexport) double* dsolve2(double* out, double* A, double* b);
}

extern "C"
{
    __declspec(dllexport) float* smvprod3(float* out, float* A, const float* x);
    __declspec(dllexport) float* ssolve3l(float* out, float* L, float* b);
    __declspec(dllexport) float* ssolve3u(float* out, float* U, float* b);
    __declspec(dllexport) float* ssolve3(float* out, float* A, float* b);

    __declspec(dllexport) double* dmvprod3(double* out, double* A, const double* x);
    __declspec(dllexport) double* dsolve3l(double* out, double* L, double* b);
    __declspec(dllexport) double* dsolve3u(double* out, double* U, double* b);
    __declspec(dllexport) double* dsolve3(double* out, double* A, double* b);
}

extern "C"
{
    __declspec(dllexport) float* smvprod4(float* out, float* A, const float* x);
    __declspec(dllexport) float* ssolve4l(float* out, float* L, float* b);
    __declspec(dllexport) float* ssolve4u(float* out, float* U, float* b);
    __declspec(dllexport) float* ssolve4(float* out, float* A, float* b);

    __declspec(dllexport) double* dmvprod4(double* out, double* A, const double* x);
    __declspec(dllexport) double* dsolve4l(double* out, double* L, double* b);
    __declspec(dllexport) double* dsolve4u(double* out, double* U, double* b);
    __declspec(dllexport) double* dsolve4(double* out, double* A, double* b);
}
