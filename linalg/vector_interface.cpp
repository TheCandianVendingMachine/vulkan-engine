#include "linalg/vector.h"
#include "linalg/vector_ops.h"

#include <cstring>

extern "C"
{
    /*
        Interface for Vector2<float> and Vector2<double>
    */
    float* saxpy2(float* out, const float a, const float* x, const float* y) {
        std::memcpy(
            out,
            linalg::blas1::axpy(a, *reinterpret_cast<const linalg::Vector2<float>*>(x), *reinterpret_cast<const linalg::Vector2<float>*>(y))
                .elements,
            sizeof(linalg::Vector2<float>::elements));
        return out;
    }
    float* sscale2(float* out, const float a, const float* x) {
        std::memcpy(out,
                    linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector2<float>*>(x)).elements,
                    sizeof(linalg::Vector2<float>::elements));
        return out;
    }
    void scopy2(float* a, const float* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector2<float>*>(a), *reinterpret_cast<const linalg::Vector2<float>*>(b));
    }
    void sswap2(float* a, float* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector2<float>*>(a), *reinterpret_cast<linalg::Vector2<float>*>(b));
    }
    float sdot2(const float* a, const float* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector2<float>*>(a), *reinterpret_cast<const linalg::Vector2<float>*>(b));
    }
    float scomponentsum2(const float* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector2<float>*>(x));
    }
    float smagnitude2(const float* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector2<float>*>(x));
    }
    float scomponentmax2(const float* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector2<float>*>(x));
    }

    double* daxpy2(double* out, const double a, const double* x, const double* y) {
        std::memcpy(out,
                    linalg::blas1::axpy(a,
                                        *reinterpret_cast<const linalg::Vector2<double>*>(x),
                                        *reinterpret_cast<const linalg::Vector2<double>*>(y))
                        .elements,
                    sizeof(linalg::Vector2<double>::elements));
        return out;
    }
    double* dscale2(double* out, const double a, const double* x) {
        std::memcpy(out,
                    linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector2<double>*>(x)).elements,
                    sizeof(linalg::Vector2<double>::elements));
        return out;
    }
    void dcopy2(double* a, const double* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector2<double>*>(a), *reinterpret_cast<const linalg::Vector2<double>*>(b));
    }
    void dswap2(double* a, double* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector2<double>*>(a), *reinterpret_cast<linalg::Vector2<double>*>(b));
    }
    double ddot2(const double* a, const double* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector2<double>*>(a),
                                  *reinterpret_cast<const linalg::Vector2<double>*>(b));
    }
    double dcomponentsum2(const double* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector2<double>*>(x));
    }
    double dmagnitude2(const double* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector2<double>*>(x));
    }
    double dcomponentmax2(const double* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector2<double>*>(x));
    }

    /*
        Interface for Vector3<float> and Vector3<double>
    */
    float* saxpy3(float* out, const float a, const float* x, const float* y) {
        std::memcpy(
            out,
            linalg::blas1::axpy(a, *reinterpret_cast<const linalg::Vector3<float>*>(x), *reinterpret_cast<const linalg::Vector3<float>*>(y))
                .elements,
            sizeof(linalg::Vector3<float>::elements));
        return out;
    }
    float* sscale3(float* out, const float a, const float* x) {
        std::memcpy(out,
                    linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector3<float>*>(x)).elements,
                    sizeof(linalg::Vector3<float>::elements));
        return out;
    }
    void scopy3(float* a, const float* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector3<float>*>(a), *reinterpret_cast<const linalg::Vector3<float>*>(b));
    }
    void sswap3(float* a, float* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector3<float>*>(a), *reinterpret_cast<linalg::Vector3<float>*>(b));
    }
    float sdot3(const float* a, const float* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector3<float>*>(a), *reinterpret_cast<const linalg::Vector3<float>*>(b));
    }
    float scomponentsum3(const float* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector3<float>*>(x));
    }
    float smagnitude3(const float* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector3<float>*>(x));
    }
    float scomponentmax3(const float* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector3<float>*>(x));
    }

    double* daxpy3(double* out, const double a, const double* x, const double* y) {
        std::memcpy(out,
                    linalg::blas1::axpy(a,
                                        *reinterpret_cast<const linalg::Vector3<double>*>(x),
                                        *reinterpret_cast<const linalg::Vector3<double>*>(y))
                        .elements,
                    sizeof(linalg::Vector3<double>::elements));
        return out;
    }
    double* dscale3(double* out, const double a, const double* x) {
        std::memcpy(out,
                    linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector3<double>*>(x)).elements,
                    sizeof(linalg::Vector3<double>::elements));
        return out;
    }
    void dcopy3(double* a, const double* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector3<double>*>(a), *reinterpret_cast<const linalg::Vector3<double>*>(b));
    }
    void dswap3(double* a, double* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector3<double>*>(a), *reinterpret_cast<linalg::Vector3<double>*>(b));
    }
    double ddot3(const double* a, const double* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector3<double>*>(a),
                                  *reinterpret_cast<const linalg::Vector3<double>*>(b));
    }
    double dcomponentsum3(const double* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector3<double>*>(x));
    }
    double dmagnitude3(const double* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector3<double>*>(x));
    }
    double dcomponentmax3(const double* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector3<double>*>(x));
    }
    /*
        Interface for Vector4<float> and Vector4<double>
    */
    float* saxpy4(float* out, const float a, const float* x, const float* y) {
        std::memcpy(
            out,
            linalg::blas1::axpy(a, *reinterpret_cast<const linalg::Vector4<float>*>(x), *reinterpret_cast<const linalg::Vector4<float>*>(y))
                .elements,
            sizeof(linalg::Vector4<float>::elements));
        return out;
    }
    float* sscale4(float* out, const float a, const float* x) {
        std::memcpy(out,
                    linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector4<float>*>(x)).elements,
                    sizeof(linalg::Vector4<float>::elements));
        return out;
    }
    void scopy4(float* a, const float* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector4<float>*>(a), *reinterpret_cast<const linalg::Vector4<float>*>(b));
    }
    void sswap4(float* a, float* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector4<float>*>(a), *reinterpret_cast<linalg::Vector4<float>*>(b));
    }
    float sdot4(const float* a, const float* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector4<float>*>(a), *reinterpret_cast<const linalg::Vector4<float>*>(b));
    }
    float scomponentsum4(const float* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector4<float>*>(x));
    }
    float smagnitude4(const float* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector4<float>*>(x));
    }
    float scomponentmax4(const float* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector4<float>*>(x));
    }

    double* daxpy4(double* out, const double a, const double* x, const double* y) {
        std::memcpy(out,
                    linalg::blas1::axpy(a,
                                        *reinterpret_cast<const linalg::Vector4<double>*>(x),
                                        *reinterpret_cast<const linalg::Vector4<double>*>(y))
                        .elements,
                    sizeof(linalg::Vector4<double>::elements));
        return out;
    }
    double* dscale4(double* out, const double a, const double* x) {
        std::memcpy(out,
                    linalg::blas1::scale(a, *reinterpret_cast<const linalg::Vector4<double>*>(x)).elements,
                    sizeof(linalg::Vector4<double>::elements));
        return out;
    }
    void dcopy4(double* a, const double* b) {
        linalg::blas1::copy(*reinterpret_cast<linalg::Vector4<double>*>(a), *reinterpret_cast<const linalg::Vector4<double>*>(b));
    }
    void dswap4(double* a, double* b) {
        linalg::blas1::swap(*reinterpret_cast<linalg::Vector4<double>*>(a), *reinterpret_cast<linalg::Vector4<double>*>(b));
    }
    double ddot4(const double* a, const double* b) {
        return linalg::blas1::dot(*reinterpret_cast<const linalg::Vector4<double>*>(a),
                                  *reinterpret_cast<const linalg::Vector4<double>*>(b));
    }
    double dcomponentsum4(const double* x) {
        return linalg::blas1::component_sum(*reinterpret_cast<const linalg::Vector4<double>*>(x));
    }
    double dmagnitude4(const double* x) {
        return linalg::blas1::magnitude(*reinterpret_cast<const linalg::Vector4<double>*>(x));
    }
    double dcomponentmax4(const double* x) {
        return linalg::blas1::component_max(*reinterpret_cast<const linalg::Vector4<double>*>(x));
    }
}
