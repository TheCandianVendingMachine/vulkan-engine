#include "linalg/matrix.h"
#include "linalg/matrix_ops.h"
#include "linalg/vector.h"
#include <cstring>

extern "C"
{
    /*
        Interface for Matrix2
    */
    __declspec(dllexport) float* smvprod2(float* out, float* A, const float* x) {
        std::memcpy(out,
                    linalg::blas2::matrix_vector_product(*reinterpret_cast<const linalg::Matrix2<float>*>(A),
                                                         *reinterpret_cast<const linalg::Vector2<float>*>(x))
                        .elements,
                    sizeof(linalg::Vector2<float>::elements));
        return out;
    }
    __declspec(dllexport) float* ssolve2l(float* out, float* L, float* b) {
        std::memcpy(out,
                    linalg::blas2::solve_lower_triangular(*reinterpret_cast<const linalg::Matrix2<float>*>(L),
                                                          *reinterpret_cast<const linalg::Vector2<float>*>(b))
                        .elements,
                    sizeof(linalg::Vector2<float>::elements));
        return out;
    }
    __declspec(dllexport) float* ssolve2u(float* out, float* U, float* b) {
        std::memcpy(out,
                    linalg::blas2::solve_upper_triangular(*reinterpret_cast<const linalg::Matrix2<float>*>(U),
                                                          *reinterpret_cast<const linalg::Vector2<float>*>(b))
                        .elements,
                    sizeof(linalg::Vector2<float>::elements));
        return out;
    }
    __declspec(dllexport) float* ssolve2(float* out, float* A, float* b) {
        std::memcpy(
            out,
            linalg::blas2::solve(*reinterpret_cast<const linalg::Matrix2<float>*>(A), *reinterpret_cast<const linalg::Vector2<float>*>(b))
                .elements,
            sizeof(linalg::Vector2<float>::elements));
        return out;
    }


    __declspec(dllexport) double* dmvprod2(double* out, double* A, const double* x) {
        std::memcpy(out,
                    linalg::blas2::matrix_vector_product(*reinterpret_cast<const linalg::Matrix2<double>*>(A),
                                                         *reinterpret_cast<const linalg::Vector2<double>*>(x))
                        .elements,
                    sizeof(linalg::Vector2<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dsolve2l(double* out, double* L, double* b) {
        std::memcpy(out,
                    linalg::blas2::solve_lower_triangular(*reinterpret_cast<const linalg::Matrix2<double>*>(L),
                                                          *reinterpret_cast<const linalg::Vector2<double>*>(b))
                        .elements,
                    sizeof(linalg::Vector2<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dsolve2u(double* out, double* U, double* b) {
        std::memcpy(out,
                    linalg::blas2::solve_upper_triangular(*reinterpret_cast<const linalg::Matrix2<double>*>(U),
                                                          *reinterpret_cast<const linalg::Vector2<double>*>(b))
                        .elements,
                    sizeof(linalg::Vector2<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dsolve2(double* out, double* A, double* b) {
        std::memcpy(
            out,
            linalg::blas2::solve(*reinterpret_cast<const linalg::Matrix2<double>*>(A), *reinterpret_cast<const linalg::Vector2<double>*>(b))
                .elements,
            sizeof(linalg::Vector2<double>::elements));
        return out;
    }


    /*
        Interface for Matrix3
    */
    __declspec(dllexport) float* smvprod3(float* out, float* A, const float* x) {
        std::memcpy(out,
                    linalg::blas2::matrix_vector_product(*reinterpret_cast<const linalg::Matrix3<float>*>(A),
                                                         *reinterpret_cast<const linalg::Vector3<float>*>(x))
                        .elements,
                    sizeof(linalg::Vector3<float>::elements));
        return out;
    }
    __declspec(dllexport) float* ssolve3l(float* out, float* L, float* b) {
        std::memcpy(out,
                    linalg::blas2::solve_lower_triangular(*reinterpret_cast<const linalg::Matrix3<float>*>(L),
                                                          *reinterpret_cast<const linalg::Vector3<float>*>(b))
                        .elements,
                    sizeof(linalg::Vector3<float>::elements));
        return out;
    }
    __declspec(dllexport) float* ssolve3u(float* out, float* U, float* b) {
        std::memcpy(out,
                    linalg::blas2::solve_upper_triangular(*reinterpret_cast<const linalg::Matrix3<float>*>(U),
                                                          *reinterpret_cast<const linalg::Vector3<float>*>(b))
                        .elements,
                    sizeof(linalg::Vector3<float>::elements));
        return out;
    }
    __declspec(dllexport) float* ssolve3(float* out, float* A, float* b) {
        std::memcpy(
            out,
            linalg::blas2::solve(*reinterpret_cast<const linalg::Matrix3<float>*>(A), *reinterpret_cast<const linalg::Vector3<float>*>(b))
                .elements,
            sizeof(linalg::Vector3<float>::elements));
        return out;
    }


    __declspec(dllexport) double* dmvprod3(double* out, double* A, const double* x) {
        std::memcpy(out,
                    linalg::blas2::matrix_vector_product(*reinterpret_cast<const linalg::Matrix3<double>*>(A),
                                                         *reinterpret_cast<const linalg::Vector3<double>*>(x))
                        .elements,
                    sizeof(linalg::Vector3<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dsolve3l(double* out, double* L, double* b) {
        std::memcpy(out,
                    linalg::blas2::solve_lower_triangular(*reinterpret_cast<const linalg::Matrix3<double>*>(L),
                                                          *reinterpret_cast<const linalg::Vector3<double>*>(b))
                        .elements,
                    sizeof(linalg::Vector3<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dsolve3u(double* out, double* U, double* b) {
        std::memcpy(out,
                    linalg::blas2::solve_upper_triangular(*reinterpret_cast<const linalg::Matrix3<double>*>(U),
                                                          *reinterpret_cast<const linalg::Vector3<double>*>(b))
                        .elements,
                    sizeof(linalg::Vector3<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dsolve3(double* out, double* A, double* b) {
        std::memcpy(
            out,
            linalg::blas2::solve(*reinterpret_cast<const linalg::Matrix3<double>*>(A), *reinterpret_cast<const linalg::Vector3<double>*>(b))
                .elements,
            sizeof(linalg::Vector3<double>::elements));
        return out;
    }


    /*
        Interface for Matrix4
    */
    __declspec(dllexport) float* smvprod4(float* out, float* A, const float* x) {
        std::memcpy(out,
                    linalg::blas2::matrix_vector_product(*reinterpret_cast<const linalg::Matrix4<float>*>(A),
                                                         *reinterpret_cast<const linalg::Vector4<float>*>(x))
                        .elements,
                    sizeof(linalg::Vector4<float>::elements));
        return out;
    }
    __declspec(dllexport) float* ssolve4l(float* out, float* L, float* b) {
        std::memcpy(out,
                    linalg::blas2::solve_lower_triangular(*reinterpret_cast<const linalg::Matrix4<float>*>(L),
                                                          *reinterpret_cast<const linalg::Vector4<float>*>(b))
                        .elements,
                    sizeof(linalg::Vector4<float>::elements));
        return out;
    }
    __declspec(dllexport) float* ssolve4u(float* out, float* U, float* b) {
        std::memcpy(out,
                    linalg::blas2::solve_upper_triangular(*reinterpret_cast<const linalg::Matrix4<float>*>(U),
                                                          *reinterpret_cast<const linalg::Vector4<float>*>(b))
                        .elements,
                    sizeof(linalg::Vector4<float>::elements));
        return out;
    }
    __declspec(dllexport) float* ssolve4(float* out, float* A, float* b) {
        std::memcpy(
            out,
            linalg::blas2::solve(*reinterpret_cast<const linalg::Matrix4<float>*>(A), *reinterpret_cast<const linalg::Vector4<float>*>(b))
                .elements,
            sizeof(linalg::Vector4<float>::elements));
        return out;
    }


    __declspec(dllexport) double* dmvprod4(double* out, double* A, const double* x) {
        std::memcpy(out,
                    linalg::blas2::matrix_vector_product(*reinterpret_cast<const linalg::Matrix4<double>*>(A),
                                                         *reinterpret_cast<const linalg::Vector4<double>*>(x))
                        .elements,
                    sizeof(linalg::Vector4<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dsolve4l(double* out, double* L, double* b) {
        std::memcpy(out,
                    linalg::blas2::solve_lower_triangular(*reinterpret_cast<const linalg::Matrix4<double>*>(L),
                                                          *reinterpret_cast<const linalg::Vector4<double>*>(b))
                        .elements,
                    sizeof(linalg::Vector4<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dsolve4u(double* out, double* U, double* b) {
        std::memcpy(out,
                    linalg::blas2::solve_upper_triangular(*reinterpret_cast<const linalg::Matrix4<double>*>(U),
                                                          *reinterpret_cast<const linalg::Vector4<double>*>(b))
                        .elements,
                    sizeof(linalg::Vector4<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dsolve4(double* out, double* A, double* b) {
        std::memcpy(
            out,
            linalg::blas2::solve(*reinterpret_cast<const linalg::Matrix4<double>*>(A), *reinterpret_cast<const linalg::Vector4<double>*>(b))
                .elements,
            sizeof(linalg::Vector4<double>::elements));
        return out;
    }
}
