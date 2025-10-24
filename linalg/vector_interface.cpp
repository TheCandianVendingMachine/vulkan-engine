#include "linalg/vector.h"
#include "linalg/vector_ops.h"
#include <cstring>

extern "C"
{
    /*
        Interface for Vector2<float> and Vector2<double>
    */
    __declspec(dllexport) float* saxpy2(float* out, const float a, const float* x, const float* y) {
        std::memcpy(
            out,
            linalg::blas1::axpy(a, *reinterpret_cast<const linalg::Vector2<float>*>(x), *reinterpret_cast<const linalg::Vector2<float>*>(y))
                .elements,
            sizeof(linalg::Vector2<float>::elements));
        return out;
    }
    __declspec(dllexport) float* sscale2(float* out, const float a, const float* x) {
        std::memcpy(out, linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector2<float>*>(x)).elements,
                    sizeof(linalg::Vector2<float>::elements));
        return out;
    }
    __declspec(dllexport) void scopy2(float* a, const float* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector2<float>*>(a), *reinterpret_cast<const linalg::Vector2<float>*>(b));
    }
    __declspec(dllexport) void sswap2(float* a, float* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector2<float>*>(a), *reinterpret_cast<linalg::Vector2<float>*>(b));
    }
    __declspec(dllexport) float sdot2(const float* a, const float* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector2<float>*>(a), *reinterpret_cast<const linalg::Vector2<float>*>(b));
    }
    __declspec(dllexport) float scomponentsum2(const float* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector2<float>*>(x));
    }
    __declspec(dllexport) float smagnitude2(const float* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector2<float>*>(x));
    }
    __declspec(dllexport) float scomponentmax2(const float* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector2<float>*>(x));
    }

    __declspec(dllexport) double* daxpy2(double* out, const double a, const double* x, const double* y) {
        std::memcpy(out,
                    linalg::blas1::axpy(a, *reinterpret_cast<const linalg::Vector2<double>*>(x),
                                        *reinterpret_cast<const linalg::Vector2<double>*>(y))
                        .elements,
                    sizeof(linalg::Vector2<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dscale2(double* out, const double a, const double* x) {
        std::memcpy(out, linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector2<double>*>(x)).elements,
                    sizeof(linalg::Vector2<double>::elements));
        return out;
    }
    __declspec(dllexport) void dcopy2(double* a, const double* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector2<double>*>(a), *reinterpret_cast<const linalg::Vector2<double>*>(b));
    }
    __declspec(dllexport) void dswap2(double* a, double* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector2<double>*>(a), *reinterpret_cast<linalg::Vector2<double>*>(b));
    }
    __declspec(dllexport) double ddot2(const double* a, const double* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector2<double>*>(a),
                                  *reinterpret_cast<const linalg::Vector2<double>*>(b));
    }
    __declspec(dllexport) double dcomponentsum2(const double* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector2<double>*>(x));
    }
    __declspec(dllexport) double dmagnitude2(const double* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector2<double>*>(x));
    }
    __declspec(dllexport) double dcomponentmax2(const double* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector2<double>*>(x));
    }

    /*
        Interface for Vector3<float> and Vector3<double>
    */
    __declspec(dllexport) float* saxpy3(float* out, const float a, const float* x, const float* y) {
        std::memcpy(
            out,
            linalg::blas1::axpy(a, *reinterpret_cast<const linalg::Vector3<float>*>(x), *reinterpret_cast<const linalg::Vector3<float>*>(y))
                .elements,
            sizeof(linalg::Vector3<float>::elements));
        return out;
    }
    __declspec(dllexport) float* sscale3(float* out, const float a, const float* x) {
        std::memcpy(out, linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector3<float>*>(x)).elements,
                    sizeof(linalg::Vector3<float>::elements));
        return out;
    }
    __declspec(dllexport) void scopy3(float* a, const float* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector3<float>*>(a), *reinterpret_cast<const linalg::Vector3<float>*>(b));
    }
    __declspec(dllexport) void sswap3(float* a, float* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector3<float>*>(a), *reinterpret_cast<linalg::Vector3<float>*>(b));
    }
    __declspec(dllexport) float sdot3(const float* a, const float* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector3<float>*>(a), *reinterpret_cast<const linalg::Vector3<float>*>(b));
    }
    __declspec(dllexport) float scomponentsum3(const float* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector3<float>*>(x));
    }
    __declspec(dllexport) float smagnitude3(const float* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector3<float>*>(x));
    }
    __declspec(dllexport) float scomponentmax3(const float* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector3<float>*>(x));
    }

    __declspec(dllexport) double* daxpy3(double* out, const double a, const double* x, const double* y) {
        std::memcpy(out,
                    linalg::blas1::axpy(a, *reinterpret_cast<const linalg::Vector3<double>*>(x),
                                        *reinterpret_cast<const linalg::Vector3<double>*>(y))
                        .elements,
                    sizeof(linalg::Vector3<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dscale3(double* out, const double a, const double* x) {
        std::memcpy(out, linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector3<double>*>(x)).elements,
                    sizeof(linalg::Vector3<double>::elements));
        return out;
    }
    __declspec(dllexport) void dcopy3(double* a, const double* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector3<double>*>(a), *reinterpret_cast<const linalg::Vector3<double>*>(b));
    }
    __declspec(dllexport) void dswap3(double* a, double* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector3<double>*>(a), *reinterpret_cast<linalg::Vector3<double>*>(b));
    }
    __declspec(dllexport) double ddot3(const double* a, const double* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector3<double>*>(a),
                                  *reinterpret_cast<const linalg::Vector3<double>*>(b));
    }
    __declspec(dllexport) double dcomponentsum3(const double* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector3<double>*>(x));
    }
    __declspec(dllexport) double dmagnitude3(const double* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector3<double>*>(x));
    }
    __declspec(dllexport) double dcomponentmax3(const double* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector3<double>*>(x));
    }
    /*
        Interface for Vector4<float> and Vector4<double>
    */
    __declspec(dllexport) float* saxpy4(float* out, const float a, const float* x, const float* y) {
        std::memcpy(
            out,
            linalg::blas1::axpy(a, *reinterpret_cast<const linalg::Vector4<float>*>(x), *reinterpret_cast<const linalg::Vector4<float>*>(y))
                .elements,
            sizeof(linalg::Vector4<float>::elements));
        return out;
    }
    __declspec(dllexport) float* sscale4(float* out, const float a, const float* x) {
        std::memcpy(out, linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector4<float>*>(x)).elements,
                    sizeof(linalg::Vector4<float>::elements));
        return out;
    }
    __declspec(dllexport) void scopy4(float* a, const float* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector4<float>*>(a), *reinterpret_cast<const linalg::Vector4<float>*>(b));
    }
    __declspec(dllexport) void sswap4(float* a, float* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector4<float>*>(a), *reinterpret_cast<linalg::Vector4<float>*>(b));
    }
    __declspec(dllexport) float sdot4(const float* a, const float* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector4<float>*>(a), *reinterpret_cast<const linalg::Vector4<float>*>(b));
    }
    __declspec(dllexport) float scomponentsum4(const float* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector4<float>*>(x));
    }
    __declspec(dllexport) float smagnitude4(const float* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector4<float>*>(x));
    }
    __declspec(dllexport) float scomponentmax4(const float* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector4<float>*>(x));
    }

    __declspec(dllexport) double* daxpy4(double* out, const double a, const double* x, const double* y) {
        std::memcpy(out,
                    linalg::blas1::axpy(a, *reinterpret_cast<const linalg::Vector4<double>*>(x),
                                        *reinterpret_cast<const linalg::Vector4<double>*>(y))
                        .elements,
                    sizeof(linalg::Vector4<double>::elements));
        return out;
    }
    __declspec(dllexport) double* dscale4(double* out, const double a, const double* x) {
        std::memcpy(out, linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector4<double>*>(x)).elements,
                    sizeof(linalg::Vector4<double>::elements));
        return out;
    }
    __declspec(dllexport) void dcopy4(double* a, const double* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector4<double>*>(a), *reinterpret_cast<const linalg::Vector4<double>*>(b));
    }
    __declspec(dllexport) void dswap4(double* a, double* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector4<double>*>(a), *reinterpret_cast<linalg::Vector4<double>*>(b));
    }
    __declspec(dllexport) double ddot4(const double* a, const double* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector4<double>*>(a),
                                  *reinterpret_cast<const linalg::Vector4<double>*>(b));
    }
    __declspec(dllexport) double dcomponentsum4(const double* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector4<double>*>(x));
    }
    __declspec(dllexport) double dmagnitude4(const double* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector4<double>*>(x));
    }
    __declspec(dllexport) double dcomponentmax4(const double* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector4<double>*>(x));
    }
}
