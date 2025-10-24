#include "engine/linalg/matrix.h"
#include "engine/linalg/vector.h"
#include "engine/meta_defines.h"
#include <Tracy/Tracy.hpp>

using namespace linalg;

/*
    Addition
*/
template <>
auto linalg::operator+ <float>(const Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float> {
    ZoneScoped;
    auto row1 = Vector4<float>::zero();
    auto row2 = Vector4<float>::zero();
    auto row3 = Vector4<float>::zero();
    auto row4 = Vector4<float>::zero();

    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row1.elements, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row2.elements, 1.f, &A.r2c1, &B.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row3.elements, 1.f, &A.r3c1, &B.r3c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row4.elements, 1.f, &A.r4c1, &B.r4c1);


    return Matrix4<float>(row1, row2, row3, row4);
}
template <>
auto linalg::operator+ <double>(const Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double> {
    ZoneScoped;
    auto row1 = Vector4<double>::zero();
    auto row2 = Vector4<double>::zero();
    auto row3 = Vector4<double>::zero();
    auto row4 = Vector4<double>::zero();

    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row1.elements, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row2.elements, 1.f, &A.r2c1, &B.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row3.elements, 1.f, &A.r3c1, &B.r3c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row4.elements, 1.f, &A.r4c1, &B.r4c1);


    return Matrix4<double>(row1, row2, row3, row4);
}

template <>
auto linalg::operator+= <float>(Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r1c1, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r2c1, 1.f, &A.r2c1, &B.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r3c1, 1.f, &A.r3c1, &B.r3c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r4c1, 1.f, &A.r4c1, &B.r4c1);


    return A;
}
template <>
auto linalg::operator+= <double>(Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r1c1, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r2c1, 1.f, &A.r2c1, &B.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r3c1, 1.f, &A.r3c1, &B.r3c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r4c1, 1.f, &A.r4c1, &B.r4c1);


    return A;
}

/*
    Subtraction
*/
template <>
auto linalg::operator- <float>(const Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float> {
    ZoneScoped;
    auto row1 = Vector4<float>::zero();
    auto row2 = Vector4<float>::zero();
    auto row3 = Vector4<float>::zero();
    auto row4 = Vector4<float>::zero();


    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row1.elements, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row2.elements, -1.f, &B.r2c1, &A.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row3.elements, -1.f, &B.r3c1, &A.r3c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row4.elements, -1.f, &B.r4c1, &A.r4c1);


    return Matrix4<float>(row1, row2, row3, row4);
}
template <>
auto linalg::operator- <double>(const Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double> {
    ZoneScoped;
    auto row1 = Vector4<double>::zero();
    auto row2 = Vector4<double>::zero();
    auto row3 = Vector4<double>::zero();
    auto row4 = Vector4<double>::zero();


    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row1.elements, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row2.elements, -1.f, &B.r2c1, &A.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row3.elements, -1.f, &B.r3c1, &A.r3c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row4.elements, -1.f, &B.r4c1, &A.r4c1);


    return Matrix4<double>(row1, row2, row3, row4);
}

template <>
auto linalg::operator-= <float>(Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r1c1, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r2c1, -1.f, &B.r2c1, &A.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r3c1, -1.f, &B.r3c1, &A.r3c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r4c1, -1.f, &B.r4c1, &A.r4c1);


    return A;
}
template <>
auto linalg::operator-= <double>(Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double>& {
    ZoneScoped;
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r1c1, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r2c1, -1.f, &B.r2c1, &A.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r3c1, -1.f, &B.r3c1, &A.r3c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r4c1, -1.f, &B.r4c1, &A.r4c1);


    return A;
}

/*
    Matrix Multiplication
*/
template <>
auto linalg::operator* <float>(const Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float> {
    ZoneScoped;
    auto col1 = Vector4<float>(B.c1r1, B.c1r2, B.c1r3, B.c1r4);
    auto col2 = Vector4<float>(B.c2r1, B.c2r2, B.c2r3, B.c2r4);
    auto col3 = Vector4<float>(B.c3r1, B.c3r2, B.c3r3, B.c3r4);
    auto col4 = Vector4<float>(B.c3r1, B.c3r2, B.c3r3, B.c4r4);


    auto r1c1 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r1, col1.elements);
    auto r1c2 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r1, col2.elements);
    auto r1c3 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r1, col3.elements);
    auto r1c4 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r1, col4.elements);


    auto r2c1 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r2, col1.elements);
    auto r2c2 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r2, col2.elements);
    auto r2c3 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r2, col3.elements);
    auto r2c4 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r2, col4.elements);


    auto r3c1 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r3, col1.elements);
    auto r3c2 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r3, col2.elements);
    auto r3c3 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r3, col3.elements);
    auto r3c4 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r3, col4.elements);

    auto r4c1 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r4, col1.elements);
    auto r4c2 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r4, col2.elements);
    auto r4c3 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r4, col3.elements);
    auto r4c4 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r4, col4.elements);


    return Matrix4<float>(r1c1, r1c2, r1c3, r1c4, r2c1, r2c2, r2c3, r2c4, r3c1, r3c2, r3c3, r3c4, r4c1, r4c2, r4c3, r4c4);
}
template <>
auto linalg::operator* <double>(const Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double> {
    ZoneScoped;
    auto col1 = Vector4<double>(B.c1r1, B.c1r2, B.c1r3, B.c1r4);
    auto col2 = Vector4<double>(B.c2r1, B.c2r2, B.c2r3, B.c2r4);
    auto col3 = Vector4<double>(B.c3r1, B.c3r2, B.c3r3, B.c3r4);
    auto col4 = Vector4<double>(B.c3r1, B.c3r2, B.c3r3, B.c4r4);


    auto r1c1 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r1, col1.elements);
    auto r1c2 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r1, col2.elements);
    auto r1c3 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r1, col3.elements);
    auto r1c4 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r1, col4.elements);


    auto r2c1 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r2, col1.elements);
    auto r2c2 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r2, col2.elements);
    auto r2c3 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r2, col3.elements);
    auto r2c4 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r2, col4.elements);


    auto r3c1 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r3, col1.elements);
    auto r3c2 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r3, col2.elements);
    auto r3c3 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r3, col3.elements);
    auto r3c4 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r3, col4.elements);

    auto r4c1 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r4, col1.elements);
    auto r4c2 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r4, col2.elements);
    auto r4c3 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r4, col3.elements);
    auto r4c4 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r4, col4.elements);


    return Matrix4<double>(r1c1, r1c2, r1c3, r1c4, r2c1, r2c2, r2c3, r2c4, r3c1, r3c2, r3c3, r3c4, r4c1, r4c2, r4c3, r4c4);
}

template <>
auto linalg::operator*= <float>(Matrix4<float>& A, const Matrix4<float>& B) -> Matrix4<float>& {
    ZoneScoped;
    A = A * B;
    return A;
}
template <>
auto linalg::operator*= <double>(Matrix4<double>& A, const Matrix4<double>& B) -> Matrix4<double>& {
    ZoneScoped;
    A = A * B;
    return A;
}

/*
    Vector Multiplication
*/
template <>
auto linalg::operator* <float>(const Matrix4<float>& A, const Vector4<float>& x) -> Vector4<float> {
    ZoneScoped;
    auto out = Vector4<float>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float32::matrix_vector_product(out.elements, const_cast<float*>(A.elements), x.elements);
    return out;
}
template <>
auto linalg::operator* <double>(const Matrix4<double>& A, const Vector4<double>& x) -> Vector4<double> {
    ZoneScoped;
    auto out = Vector4<double>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float64::matrix_vector_product(out.elements, const_cast<double*>(A.elements), x.elements);
    return out;
}

/*
    Solving
*/
template <>
auto linalg::operator/ <float>(const Matrix4<float>& A, const Vector4<float>& x) -> Vector4<float> {
    ZoneScoped;
    auto out = Vector4<float>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float32::solve(const_cast<float*>(out.elements), const_cast<float*>(A.elements),
                                                      const_cast<float*>(x.elements));
    return out;
}
template <>
auto linalg::operator/ <double>(const Matrix4<double>& A, const Vector4<double>& x) -> Vector4<double> {
    ZoneScoped;
    auto out = Vector4<double>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float64::solve(const_cast<double*>(out.elements), const_cast<double*>(A.elements),
                                                      const_cast<double*>(x.elements));
    return out;
}
