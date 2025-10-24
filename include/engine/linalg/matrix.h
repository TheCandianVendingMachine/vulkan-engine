#pragma once
#include "engine/meta_defines.h"
#include "linalg/matrix.h"
#include "linalg/vector.h"

// Matrix2
namespace linalg {
    /*
        Addition
    */
    template <typename T>
    auto operator+(const Matrix2<T>& A, const Matrix2<T>& B) -> Matrix2<T>;
    template <>
    auto operator+ <float>(const Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float>;
    template <>
    auto operator+ <double>(const Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double>;

    template <typename T>
    auto operator+=(Matrix2<T>& A, const Matrix2<T>& B) -> Matrix2<T>&;
    template <>
    auto operator+= <float>(Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float>&;
    template <>
    auto operator+= <double>(Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double>&;

    /*
        Subtraction
    */
    template <typename T>
    auto operator-(const Matrix2<T>& A, const Matrix2<T>& B) -> Matrix2<T>;
    template <>
    auto operator- <float>(const Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float>;
    template <>
    auto operator- <double>(const Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double>;

    template <typename T>
    auto operator-=(Matrix2<T>& A, const Matrix2<T>& B) -> Matrix2<T>&;
    template <>
    auto operator-= <float>(Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float>&;
    template <>
    auto operator-= <double>(Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double>&;

    /*
        Matrix Multiplication
    */
    template <typename T>
    auto operator*(const Matrix2<T>& A, const Matrix2<T>& B) -> Matrix2<T>;
    template <>
    auto operator* <float>(const Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float>;
    template <>
    auto operator* <double>(const Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double>;

    template <typename T>
    auto operator*=(Matrix2<T>& A, const Matrix2<T>& B) -> Matrix2<T>&;
    template <>
    auto operator*= <float>(Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float>&;
    template <>
    auto operator*= <double>(Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double>&;

    /*
        Vector Multiplication
    */
    template <typename T>
    auto operator*(const Matrix2<T>& A, const Vector2<T>& x) -> Vector2<T> = delete;
    template <>
    auto operator* <float>(const Matrix2<float>& A, const Vector2<float>& x) -> Vector2<float>;
    template <>
    auto operator* <double>(const Matrix2<double>& A, const Vector2<double>& x) -> Vector2<double>;

    /*
        Solving
    */
    template <typename T>
    auto operator/(const Matrix2<T>& A, const Vector2<T>& x) -> Vector2<T> = delete;
    template <>
    auto operator/ <float>(const Matrix2<float>& A, const Vector2<float>& x) -> Vector2<float>;
    template <>
    auto operator/ <double>(const Matrix2<double>& A, const Vector2<double>& x) -> Vector2<double>;
} // namespace linalg
// Matrix3
namespace linalg {
    /*
        Addition
    */
    template <typename T>
    auto operator+(const Matrix3<T>& A, const Matrix3<T>& B) -> Matrix3<T>;
    template <>
    auto operator+ <float>(const Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float>;
    template <>
    auto operator+ <double>(const Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double>;

    template <typename T>
    auto operator+=(Matrix3<T>& A, const Matrix3<T>& B) -> Matrix3<T>&;
    template <>
    auto operator+= <float>(Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float>&;
    template <>
    auto operator+= <double>(Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double>&;

    /*
        Subtraction
    */
    template <typename T>
    auto operator-(const Matrix3<T>& A, const Matrix3<T>& B) -> Matrix3<T>;
    template <>
    auto operator- <float>(const Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float>;
    template <>
    auto operator- <double>(const Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double>;

    template <typename T>
    auto operator-=(Matrix3<T>& A, const Matrix3<T>& B) -> Matrix3<T>&;
    template <>
    auto operator-= <float>(Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float>&;
    template <>
    auto operator-= <double>(Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double>&;

    /*
        Matrix Multiplication
    */
    template <typename T>
    auto operator*(const Matrix3<T>& A, const Matrix3<T>& B) -> Matrix3<T>;
    template <>
    auto operator* <float>(const Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float>;
    template <>
    auto operator* <double>(const Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double>;

    template <typename T>
    auto operator*=(Matrix3<T>& A, const Matrix3<T>& B) -> Matrix3<T>&;
    template <>
    auto operator*= <float>(Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float>&;
    template <>
    auto operator*= <double>(Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double>&;

    /*
        Vector Multiplication
    */
    template <typename T>
    auto operator*(const Matrix3<T>& A, const Vector3<T>& x) -> Vector3<T> = delete;
    template <>
    auto operator* <float>(const Matrix3<float>& A, const Vector3<float>& x) -> Vector3<float>;
    template <>
    auto operator* <double>(const Matrix3<double>& A, const Vector3<double>& x) -> Vector3<double>;

    /*
        Solving
    */
    template <typename T>
    auto operator/(const Matrix3<T>& A, const Vector3<T>& x) -> Vector3<T> = delete;
    template <>
    auto operator/ <float>(const Matrix3<float>& A, const Vector3<float>& x) -> Vector3<float>;
    template <>
    auto operator/ <double>(const Matrix3<double>& A, const Vector3<double>& x) -> Vector3<double>;
} // namespace linalg
// Matrix4
namespace linalg {
    /*
        Addition
    */
    template <typename T>
    auto operator+(const Matrix4<T>& A, const Matrix4<T>& B) -> Matrix4<T>;
    template <>
    auto operator+ <float>(const Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float>;
    template <>
    auto operator+ <double>(const Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double>;

    template <typename T>
    auto operator+=(Matrix4<T>& A, const Matrix4<T>& B) -> Matrix4<T>&;
    template <>
    auto operator+= <float>(Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float>&;
    template <>
    auto operator+= <double>(Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double>&;

    /*
        Subtraction
    */
    template <typename T>
    auto operator-(const Matrix4<T>& A, const Matrix4<T>& B) -> Matrix4<T>;
    template <>
    auto operator- <float>(const Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float>;
    template <>
    auto operator- <double>(const Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double>;

    template <typename T>
    auto operator-=(Matrix4<T>& A, const Matrix4<T>& B) -> Matrix4<T>&;
    template <>
    auto operator-= <float>(Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float>&;
    template <>
    auto operator-= <double>(Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double>&;

    /*
        Matrix Multiplication
    */
    template <typename T>
    auto operator*(const Matrix4<T>& A, const Matrix4<T>& B) -> Matrix4<T>;
    template <>
    auto operator* <float>(const Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float>;
    template <>
    auto operator* <double>(const Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double>;

    template <typename T>
    auto operator*=(Matrix4<T>& A, const Matrix4<T>& B) -> Matrix4<T>&;
    template <>
    auto operator*= <float>(Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float>&;
    template <>
    auto operator*= <double>(Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double>&;

    /*
        Vector Multiplication
    */
    template <typename T>
    auto operator*(const Matrix4<T>& A, const Vector4<T>& x) -> Vector4<T> = delete;
    template <>
    auto operator* <float>(const Matrix4<float>& A, const Vector4<float>& x) -> Vector4<float>;
    template <>
    auto operator* <double>(const Matrix4<double>& A, const Vector4<double>& x) -> Vector4<double>;

    /*
        Solving
    */
    template <typename T>
    auto operator/(const Matrix4<T>& A, const Vector4<T>& x) -> Vector4<T> = delete;
    template <>
    auto operator/ <float>(const Matrix4<float>& A, const Vector4<float>& x) -> Vector4<float>;
    template <>
    auto operator/ <double>(const Matrix4<double>& A, const Vector4<double>& x) -> Vector4<double>;
} // namespace linalg


namespace ENGINE_NS {
    namespace linalg {
        void load_matrix_functions(const void* library);
        namespace blas2 {
            namespace matrix2 {
                namespace float32 {
                    typedef float*(__cdecl* matrix_vector_product_def)(float*, float*, const float*);
                    typedef float*(__cdecl* solve_lower_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_upper_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_def)(float*, float*, float*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* matrix_vector_product_def)(double*, double*, const double*);
                    typedef double*(__cdecl* solve_lower_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_upper_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_def)(double*, double*, double*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float64
            } // namespace matrix2
            namespace matrix3 {
                namespace float32 {
                    typedef float*(__cdecl* matrix_vector_product_def)(float*, float*, const float*);
                    typedef float*(__cdecl* solve_lower_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_upper_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_def)(float*, float*, float*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* matrix_vector_product_def)(double*, double*, const double*);
                    typedef double*(__cdecl* solve_lower_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_upper_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_def)(double*, double*, double*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float64
            } // namespace matrix3
            namespace matrix4 {
                namespace float32 {
                    typedef float*(__cdecl* matrix_vector_product_def)(float*, float*, const float*);
                    typedef float*(__cdecl* solve_lower_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_upper_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_def)(float*, float*, float*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* matrix_vector_product_def)(double*, double*, const double*);
                    typedef double*(__cdecl* solve_lower_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_upper_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_def)(double*, double*, double*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float64
            } // namespace matrix4
        } // namespace blas2
    } // namespace linalg
} // namespace ENGINE_NS
