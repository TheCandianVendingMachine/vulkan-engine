#pragma once
#include "engine/meta_defines.h"
#include <linalg/vector.h>

// Vector2
namespace linalg {
    // Addition
    template <typename T>
    auto operator+(const Vector2<T>& lhs, const Vector2<T>& rhs) -> Vector2<T>;
    template <>
    auto operator+ <float>(const Vector2<float>& lhs, const Vector2<float>& rhs) -> Vector2<float>;
    template <>
    auto operator+ <double>(const Vector2<double>& lhs, const Vector2<double>& rhs) -> Vector2<double>;

    template <typename T>
    auto operator+=(Vector2<T>& lhs, const Vector2<T>& rhs) -> Vector2<T>& = delete;
    template <>
    auto operator+= <float>(Vector2<float>& lhs, const Vector2<float>& rhs) -> Vector2<float>&;
    template <>
    auto operator+= <double>(Vector2<double>& lhs, const Vector2<double>& rhs) -> Vector2<double>&;

    // Subtraction
    template <typename T>
    auto operator-(const Vector2<T>& lhs, const Vector2<T>& rhs) -> Vector2<T> = delete;
    template <>
    auto operator- <float>(const Vector2<float>& lhs, const Vector2<float>& rhs) -> Vector2<float>;
    template <>
    auto operator- <double>(const Vector2<double>& lhs, const Vector2<double>& rhs) -> Vector2<double>;

    template <typename T>
    auto operator-=(Vector2<T>& lhs, const Vector2<T>& rhs) -> Vector2<T>& = delete;
    template <>
    auto operator-= <float>(Vector2<float>& lhs, const Vector2<float>& rhs) -> Vector2<float>&;
    template <>
    auto operator-= <double>(Vector2<double>& lhs, const Vector2<double>& rhs) -> Vector2<double>&;

    // Scalar Multiplication
    template <typename T>
    auto operator*(const Vector2<T>& lhs, T rhs) -> Vector2<T> = delete;
    template <>
    auto operator* <float>(const Vector2<float>& lhs, float rhs) -> Vector2<float>;
    template <>
    auto operator* <double>(const Vector2<double>& lhs, double rhs) -> Vector2<double>;

    template <typename T>
    auto operator*(T lhs, const Vector2<T>& rhs) -> Vector2<T> = delete;
    template <>
    auto operator* <float>(float lhs, const Vector2<float>& rhs) -> Vector2<float>;
    template <>
    auto operator* <double>(double lhs, const Vector2<double>& rhs) -> Vector2<double>;

    template <typename T>
    auto operator*=(Vector2<T>& lhs, T rhs) -> Vector2<T>& = delete;
    template <>
    auto operator*= <float>(Vector2<float>& lhs, float rhs) -> Vector2<float>&;
    template <>
    auto operator*= <double>(Vector2<double>& lhs, double rhs) -> Vector2<double>&;

    template <typename T>
    auto operator*=(T lhs, Vector2<T>& rhs) -> Vector2<T>& = delete;
    template <>
    auto operator*= <float>(float lhs, Vector2<float>& rhs) -> Vector2<float>&;
    template <>
    auto operator*= <double>(double lhs, Vector2<double>& rhs) -> Vector2<double>&;

    // Division
    template <typename T>
    auto operator/(const Vector2<T>& lhs, T rhs) -> Vector2<T> = delete;
    template <>
    auto operator/ <float>(const Vector2<float>& lhs, float rhs) -> Vector2<float>;
    template <>
    auto operator/ <double>(const Vector2<double>& lhs, double rhs) -> Vector2<double>;

    template <typename T>
    auto operator/=(Vector2<T>& lhs, T rhs) -> Vector2<T>& = delete;
    template <>
    auto operator/= <float>(Vector2<float>& lhs, float rhs) -> Vector2<float>&;
    template <>
    auto operator/= <double>(Vector2<double>& lhs, double rhs) -> Vector2<double>&;

    // Dot
    template <typename T>
    auto dot(const Vector2<T>& lhs, const Vector2<T>& rhs) -> T = delete;
    template <>
    auto dot<float>(const Vector2<float>& lhs, const Vector2<float>& rhs) -> float;
    template <>
    auto dot<double>(const Vector2<double>& lhs, const Vector2<double>& rhs) -> double;

    // Magnitude
    template <typename T>
    auto magnitude(const Vector2<T>& a) -> T = delete;
    template <>
    auto magnitude<float>(const Vector2<float>& a) -> float;
    template <>
    auto magnitude<double>(const Vector2<double>& a) -> double;

    // Magnitude sqr
    template <typename T>
    auto magnitude_sqr(const Vector2<T>& a) -> T = delete;
    template <>
    auto magnitude_sqr<float>(const Vector2<float>& a) -> float;
    template <>
    auto magnitude_sqr<double>(const Vector2<double>& a) -> double;

    // Project
    template <typename T>
    auto project(const Vector2<T>& a, const Vector2<T>& b) -> Vector2<T> = delete;
    template <>
    auto project<float>(const Vector2<float>& a, const Vector2<float>& b) -> Vector2<float>;
    template <>
    auto project<double>(const Vector2<double>& a, const Vector2<double>& b) -> Vector2<double>;

    // Normalize
    template <typename T>
    auto normalize(const Vector2<T>& a) -> Vector2<T> = delete;
    template <>
    auto normalize<float>(const Vector2<float>& a) -> Vector2<float>;
    template <>
    auto normalize<double>(const Vector2<double>& a) -> Vector2<double>;

    template <typename T>
    auto normalize_inplace(Vector2<T>& a) -> Vector2<T>& = delete;
    template <>
    auto normalize_inplace<float>(Vector2<float>& a) -> Vector2<float>&;
    template <>
    auto normalize_inplace<double>(Vector2<double>& a) -> Vector2<double>&;
} // namespace linalg
// Vector3
namespace linalg {
    // Addition
    template <typename T>
    auto operator+(const Vector3<T>& lhs, const Vector3<T>& rhs) -> Vector3<T> = delete;
    template <>
    auto operator+ <float>(const Vector3<float>& lhs, const Vector3<float>& rhs) -> Vector3<float>;
    template <>
    auto operator+ <double>(const Vector3<double>& lhs, const Vector3<double>& rhs) -> Vector3<double>;

    template <typename T>
    auto operator+=(Vector3<T>& lhs, const Vector3<T>& rhs) -> Vector3<T>& = delete;
    template <>
    auto operator+= <float>(Vector3<float>& lhs, const Vector3<float>& rhs) -> Vector3<float>&;
    template <>
    auto operator+= <double>(Vector3<double>& lhs, const Vector3<double>& rhs) -> Vector3<double>&;

    // Subtraction
    template <typename T>
    auto operator-(const Vector3<T>& lhs, const Vector3<T>& rhs) -> Vector3<T> = delete;
    template <>
    auto operator- <float>(const Vector3<float>& lhs, const Vector3<float>& rhs) -> Vector3<float>;
    template <>
    auto operator- <double>(const Vector3<double>& lhs, const Vector3<double>& rhs) -> Vector3<double>;

    template <typename T>
    auto operator-=(Vector3<T>& lhs, const Vector3<T>& rhs) -> Vector3<T>& = delete;
    template <>
    auto operator-= <float>(Vector3<float>& lhs, const Vector3<float>& rhs) -> Vector3<float>&;
    template <>
    auto operator-= <double>(Vector3<double>& lhs, const Vector3<double>& rhs) -> Vector3<double>&;

    // Scalar Multiplication
    template <typename T>
    auto operator*(const Vector3<T>& lhs, T rhs) -> Vector3<T> = delete;
    template <>
    auto operator* <float>(const Vector3<float>& lhs, float rhs) -> Vector3<float>;
    template <>
    auto operator* <double>(const Vector3<double>& lhs, double rhs) -> Vector3<double>;

    template <typename T>
    auto operator*(T lhs, const Vector3<T>& rhs) -> Vector3<T> = delete;
    template <>
    auto operator* <float>(float lhs, const Vector3<float>& rhs) -> Vector3<float>;
    template <>
    auto operator* <double>(double lhs, const Vector3<double>& rhs) -> Vector3<double>;

    template <typename T>
    auto operator*=(Vector3<T>& lhs, T rhs) -> Vector3<T>& = delete;
    template <>
    auto operator*= <float>(Vector3<float>& lhs, float rhs) -> Vector3<float>&;
    template <>
    auto operator*= <double>(Vector3<double>& lhs, double rhs) -> Vector3<double>&;

    template <typename T>
    auto operator*=(T lhs, Vector3<T>& rhs) -> Vector3<T>& = delete;
    template <>
    auto operator*= <float>(float lhs, Vector3<float>& rhs) -> Vector3<float>&;
    template <>
    auto operator*= <double>(double lhs, Vector3<double>& rhs) -> Vector3<double>&;

    // Division
    template <typename T>
    auto operator/(const Vector3<T>& lhs, T rhs) -> Vector3<T> = delete;
    template <>
    auto operator/ <float>(const Vector3<float>& lhs, float rhs) -> Vector3<float>;
    template <>
    auto operator/ <double>(const Vector3<double>& lhs, double rhs) -> Vector3<double>;

    template <typename T>
    auto operator/=(Vector3<T>& lhs, T rhs) -> Vector3<T>& = delete;
    template <>
    auto operator/= <float>(Vector3<float>& lhs, float rhs) -> Vector3<float>&;
    template <>
    auto operator/= <double>(Vector3<double>& lhs, double rhs) -> Vector3<double>&;

    // Dot
    template <typename T>
    auto dot(const Vector3<T>& lhs, const Vector3<T>& rhs) -> T = delete;
    template <>
    auto dot<float>(const Vector3<float>& lhs, const Vector3<float>& rhs) -> float;
    template <>
    auto dot<double>(const Vector3<double>& lhs, const Vector3<double>& rhs) -> double;

    // Magnitude
    template <typename T>
    auto magnitude(const Vector3<T>& a) -> T = delete;
    template <>
    auto magnitude<float>(const Vector3<float>& a) -> float;
    template <>
    auto magnitude<double>(const Vector3<double>& a) -> double;

    // Magnitude sqr
    template <typename T>
    auto magnitude_sqr(const Vector3<T>& a) -> T = delete;
    template <>
    auto magnitude_sqr<float>(const Vector3<float>& a) -> float;
    template <>
    auto magnitude_sqr<double>(const Vector3<double>& a) -> double;

    // Project
    template <typename T>
    auto project(const Vector3<T>& a, const Vector3<T>& b) -> Vector3<T> = delete;
    template <>
    auto project<float>(const Vector3<float>& a, const Vector3<float>& b) -> Vector3<float>;
    template <>
    auto project<double>(const Vector3<double>& a, const Vector3<double>& b) -> Vector3<double>;

    // Normalize
    template <typename T>
    auto normalize(const Vector3<T>& a) -> Vector3<T> = delete;
    template <>
    auto normalize<float>(const Vector3<float>& a) -> Vector3<float>;
    template <>
    auto normalize<double>(const Vector3<double>& a) -> Vector3<double>;

    template <typename T>
    auto normalize_inplace(Vector3<T>& a) -> Vector3<T>& = delete;
    template <>
    auto normalize_inplace<float>(Vector3<float>& a) -> Vector3<float>&;
    template <>
    auto normalize_inplace<double>(Vector3<double>& a) -> Vector3<double>&;

    // Cross product
    template <typename T>
    auto cross(const Vector3<T>& a, const Vector3<T>& b) -> Vector3<T> = delete;
    template <>
    auto cross<float>(const Vector3<float>& a, const Vector3<float>& b) -> Vector3<float>;
    template <>
    auto cross<double>(const Vector3<double>& a, const Vector3<double>& b) -> Vector3<double>;
} // namespace linalg
// Vector4
namespace linalg {
    // Addition
    template <typename T>
    auto operator+(const Vector4<T>& lhs, const Vector4<T>& rhs) -> Vector4<T> = delete;
    template <>
    auto operator+ <float>(const Vector4<float>& lhs, const Vector4<float>& rhs) -> Vector4<float>;
    template <>
    auto operator+ <double>(const Vector4<double>& lhs, const Vector4<double>& rhs) -> Vector4<double>;

    template <typename T>
    auto operator+=(Vector4<T>& lhs, const Vector4<T>& rhs) -> Vector4<T>& = delete;
    template <>
    auto operator+= <float>(Vector4<float>& lhs, const Vector4<float>& rhs) -> Vector4<float>&;
    template <>
    auto operator+= <double>(Vector4<double>& lhs, const Vector4<double>& rhs) -> Vector4<double>&;

    // Subtraction
    template <typename T>
    auto operator-(const Vector4<T>& lhs, const Vector4<T>& rhs) -> Vector4<T> = delete;
    template <>
    auto operator- <float>(const Vector4<float>& lhs, const Vector4<float>& rhs) -> Vector4<float>;
    template <>
    auto operator- <double>(const Vector4<double>& lhs, const Vector4<double>& rhs) -> Vector4<double>;

    template <typename T>
    auto operator-=(Vector4<T>& lhs, const Vector4<T>& rhs) -> Vector4<T>& = delete;
    template <>
    auto operator-= <float>(Vector4<float>& lhs, const Vector4<float>& rhs) -> Vector4<float>&;
    template <>
    auto operator-= <double>(Vector4<double>& lhs, const Vector4<double>& rhs) -> Vector4<double>&;

    // Scalar Multiplication
    template <typename T>
    auto operator*(const Vector4<T>& lhs, T rhs) -> Vector4<T> = delete;
    template <>
    auto operator* <float>(const Vector4<float>& lhs, float rhs) -> Vector4<float>;
    template <>
    auto operator* <double>(const Vector4<double>& lhs, double rhs) -> Vector4<double>;

    template <typename T>
    auto operator*(T lhs, const Vector4<T>& rhs) -> Vector4<T> = delete;
    template <>
    auto operator* <float>(float lhs, const Vector4<float>& rhs) -> Vector4<float>;
    template <>
    auto operator* <double>(double lhs, const Vector4<double>& rhs) -> Vector4<double>;

    template <typename T>
    auto operator*=(Vector4<T>& lhs, T rhs) -> Vector4<T>& = delete;
    template <>
    auto operator*= <float>(Vector4<float>& lhs, float rhs) -> Vector4<float>&;
    template <>
    auto operator*= <double>(Vector4<double>& lhs, double rhs) -> Vector4<double>&;

    template <typename T>
    auto operator*=(T lhs, Vector4<T>& rhs) -> Vector4<T>& = delete;
    template <>
    auto operator*= <float>(float lhs, Vector4<float>& rhs) -> Vector4<float>&;
    template <>
    auto operator*= <double>(double lhs, Vector4<double>& rhs) -> Vector4<double>&;

    // Division
    template <typename T>
    auto operator/(const Vector4<T>& lhs, T rhs) -> Vector4<T> = delete;
    template <>
    auto operator/ <float>(const Vector4<float>& lhs, float rhs) -> Vector4<float>;
    template <>
    auto operator/ <double>(const Vector4<double>& lhs, double rhs) -> Vector4<double>;

    template <typename T>
    auto operator/=(Vector4<T>& lhs, T rhs) -> Vector4<T>& = delete;
    template <>
    auto operator/= <float>(Vector4<float>& lhs, float rhs) -> Vector4<float>&;
    template <>
    auto operator/= <double>(Vector4<double>& lhs, double rhs) -> Vector4<double>&;

    // Dot
    template <typename T>
    auto dot(const Vector4<T>& lhs, const Vector4<T>& rhs) -> T = delete;
    template <>
    auto dot<float>(const Vector4<float>& lhs, const Vector4<float>& rhs) -> float;
    template <>
    auto dot<double>(const Vector4<double>& lhs, const Vector4<double>& rhs) -> double;

    // Magnitude
    template <typename T>
    auto magnitude(const Vector4<T>& a) -> T = delete;
    template <>
    auto magnitude<float>(const Vector4<float>& a) -> float;
    template <>
    auto magnitude<double>(const Vector4<double>& a) -> double;

    // Magnitude sqr
    template <typename T>
    auto magnitude_sqr(const Vector4<T>& a) -> T = delete;
    template <>
    auto magnitude_sqr<float>(const Vector4<float>& a) -> float;
    template <>
    auto magnitude_sqr<double>(const Vector4<double>& a) -> double;

    // Project
    template <typename T>
    auto project(const Vector4<T>& a, const Vector4<T>& b) -> Vector4<T> = delete;
    template <>
    auto project<float>(const Vector4<float>& a, const Vector4<float>& b) -> Vector4<float>;
    template <>
    auto project<double>(const Vector4<double>& a, const Vector4<double>& b) -> Vector4<double>;

    // Normalize
    template <typename T>
    auto normalize(const Vector4<T>& a) -> Vector4<T> = delete;
    template <>
    auto normalize<float>(const Vector4<float>& a) -> Vector4<float>;
    template <>
    auto normalize<double>(const Vector4<double>& a) -> Vector4<double>;

    template <typename T>
    auto normalize_inplace(Vector4<T>& a) -> Vector4<T>& = delete;
    template <>
    auto normalize_inplace<float>(Vector4<float>& a) -> Vector4<float>&;
    template <>
    auto normalize_inplace<double>(Vector4<double>& a) -> Vector4<double>&;
} // namespace linalg

namespace ENGINE_NS {
    namespace linalg {
        void load_vector_functions(const void* library);
        namespace blas1 {
            namespace vector2 {
                namespace float32 {
                    typedef float*(__cdecl* axpy_def)(float*, const float, const float*, const float*);
                    typedef float*(__cdecl* scale_def)(float*, const float, const float*);
                    typedef void(__cdecl* copy_def)(float*, const float*);
                    typedef void(__cdecl* swap_def)(float*, float*);
                    typedef float(__cdecl* dot_def)(const float*, const float*);
                    typedef float(__cdecl* component_sum_def)(const float*);
                    typedef float(__cdecl* magnitude_def)(const float*);
                    typedef float(__cdecl* component_max_def)(const float*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* axpy_def)(double*, const double, const double*, const double*);
                    typedef double*(__cdecl* scale_def)(double*, const double, const double*);
                    typedef void(__cdecl* copy_def)(double*, const double*);
                    typedef void(__cdecl* swap_def)(double*, double*);
                    typedef double(__cdecl* dot_def)(const double*, const double*);
                    typedef double(__cdecl* component_sum_def)(const double*);
                    typedef double(__cdecl* magnitude_def)(const double*);
                    typedef double(__cdecl* component_max_def)(const double*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float64
            } // namespace vector2
            namespace vector3 {
                namespace float32 {
                    typedef float*(__cdecl* axpy_def)(float*, const float, const float*, const float*);
                    typedef float*(__cdecl* scale_def)(float*, const float, const float*);
                    typedef void(__cdecl* copy_def)(float*, const float*);
                    typedef void(__cdecl* swap_def)(float*, float*);
                    typedef float(__cdecl* dot_def)(const float*, const float*);
                    typedef float(__cdecl* component_sum_def)(const float*);
                    typedef float(__cdecl* magnitude_def)(const float*);
                    typedef float(__cdecl* component_max_def)(const float*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* axpy_def)(double*, const double, const double*, const double*);
                    typedef double*(__cdecl* scale_def)(double*, const double, const double*);
                    typedef void(__cdecl* copy_def)(double*, const double*);
                    typedef void(__cdecl* swap_def)(double*, double*);
                    typedef double(__cdecl* dot_def)(const double*, const double*);
                    typedef double(__cdecl* component_sum_def)(const double*);
                    typedef double(__cdecl* magnitude_def)(const double*);
                    typedef double(__cdecl* component_max_def)(const double*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float64
            } // namespace vector3
            namespace vector4 {
                namespace float32 {
                    typedef float*(__cdecl* axpy_def)(float*, const float, const float*, const float*);
                    typedef float*(__cdecl* scale_def)(float*, const float, const float*);
                    typedef void(__cdecl* copy_def)(float*, const float*);
                    typedef void(__cdecl* swap_def)(float*, float*);
                    typedef float(__cdecl* dot_def)(const float*, const float*);
                    typedef float(__cdecl* component_sum_def)(const float*);
                    typedef float(__cdecl* magnitude_def)(const float*);
                    typedef float(__cdecl* component_max_def)(const float*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* axpy_def)(double*, const double, const double*, const double*);
                    typedef double*(__cdecl* scale_def)(double*, const double, const double*);
                    typedef void(__cdecl* copy_def)(double*, const double*);
                    typedef void(__cdecl* swap_def)(double*, double*);
                    typedef double(__cdecl* dot_def)(const double*, const double*);
                    typedef double(__cdecl* component_sum_def)(const double*);
                    typedef double(__cdecl* magnitude_def)(const double*);
                    typedef double(__cdecl* component_max_def)(const double*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float64
            } // namespace vector4
        } // namespace blas1
    } // namespace linalg
} // namespace ENGINE_NS
