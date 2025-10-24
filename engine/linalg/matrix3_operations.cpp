#include "engine/linalg/matrix.h"
#include "engine/linalg/vector.h"
#include "engine/meta_defines.h"

using namespace linalg;

/*
    Addition
*/
template <>
auto linalg::operator+ <float>(const Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float> {
    auto row1 = Vector3<float>::zero();
    auto row2 = Vector3<float>::zero();
    auto row3 = Vector3<float>::zero();

    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row1.elements, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row2.elements, 1.f, &A.r2c1, &B.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row3.elements, 1.f, &A.r3c1, &B.r3c1);


    return Matrix3<float>(row1, row2, row3);
}
template <>
auto linalg::operator+ <double>(const Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double> {
    auto row1 = Vector3<double>::zero();
    auto row2 = Vector3<double>::zero();
    auto row3 = Vector3<double>::zero();

    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row1.elements, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row2.elements, 1.f, &A.r2c1, &B.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row3.elements, 1.f, &A.r3c1, &B.r3c1);


    return Matrix3<double>(row1, row2, row3);
}

template <>
auto linalg::operator+= <float>(Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float>& {
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r1c1, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r2c1, 1.f, &A.r2c1, &B.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r3c1, 1.f, &A.r3c1, &B.r3c1);

    return A;
}
template <>
auto linalg::operator+= <double>(Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double>& {
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r1c1, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r2c1, 1.f, &A.r2c1, &B.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r3c1, 1.f, &A.r3c1, &B.r3c1);

    return A;
}

/*
    Subtraction
*/
template <>
auto linalg::operator- <float>(const Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float> {
    auto row1 = Vector3<float>::zero();
    auto row2 = Vector3<float>::zero();
    auto row3 = Vector3<float>::zero();

    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row1.elements, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row2.elements, -1.f, &B.r2c1, &A.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row3.elements, -1.f, &B.r3c1, &A.r3c1);


    return Matrix3<float>(row1, row2, row3);
}
template <>
auto linalg::operator- <double>(const Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double> {
    auto row1 = Vector3<double>::zero();
    auto row2 = Vector3<double>::zero();
    auto row3 = Vector3<double>::zero();

    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row1.elements, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row2.elements, -1.f, &B.r2c1, &A.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row3.elements, -1.f, &B.r3c1, &A.r3c1);


    return Matrix3<double>(row1, row2, row3);
}

template <>
auto linalg::operator-= <float>(Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float>& {
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r1c1, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r2c1, -1.f, &B.r2c1, &A.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r3c1, -1.f, &B.r3c1, &A.r3c1);

    return A;
}
template <>
auto linalg::operator-= <double>(Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double>& {
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r1c1, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r2c1, -1.f, &B.r2c1, &A.r2c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r3c1, -1.f, &B.r3c1, &A.r3c1);

    return A;
}

/*
    Matrix Multiplication
*/
template <>
auto linalg::operator* <float>(const Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float> {
    auto col1 = Vector3<float>(B.c1r1, B.c1r2, B.c1r3);
    auto col2 = Vector3<float>(B.c2r1, B.c2r2, B.c2r3);
    auto col3 = Vector3<float>(B.c3r1, B.c3r2, B.c3r3);

    auto r1c1 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r1, col1.elements);
    auto r1c2 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r1, col2.elements);
    auto r1c3 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r1, col3.elements);


    auto r2c1 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r2, col1.elements);
    auto r2c2 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r2, col2.elements);
    auto r2c3 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r2, col3.elements);


    auto r3c1 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r3, col1.elements);
    auto r3c2 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r3, col2.elements);
    auto r3c3 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r3, col3.elements);

    return Matrix3<float>(r1c1, r1c2, r1c3, r2c1, r2c2, r2c3, r3c1, r3c2, r3c3);
}
template <>
auto linalg::operator* <double>(const Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double> {
    auto col1 = Vector3<double>(B.c1r1, B.c1r2, B.c1r3);
    auto col2 = Vector3<double>(B.c2r1, B.c2r2, B.c2r3);
    auto col3 = Vector3<double>(B.c3r1, B.c3r2, B.c3r3);

    auto r1c1 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r1, col1.elements);
    auto r1c2 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r1, col2.elements);
    auto r1c3 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r1, col3.elements);


    auto r2c1 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r2, col1.elements);
    auto r2c2 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r2, col2.elements);
    auto r2c3 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r2, col3.elements);


    auto r3c1 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r3, col1.elements);
    auto r3c2 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r3, col2.elements);
    auto r3c3 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r3, col3.elements);

    return Matrix3<double>(r1c1, r1c2, r1c3, r2c1, r2c2, r2c3, r3c1, r3c2, r3c3);
}

template <>
auto linalg::operator*= <float>(Matrix3<float>& A, const Matrix3<float>& B) -> Matrix3<float>& {
    A = A * B;
    return A;
}
template <>
auto linalg::operator*= <double>(Matrix3<double>& A, const Matrix3<double>& B) -> Matrix3<double>& {
    A = A * B;
    return A;
}

/*
    Vector Multiplication
*/
template <>
auto linalg::operator* <float>(const Matrix3<float>& A, const Vector3<float>& x) -> Vector3<float> {
    auto out = Vector3<float>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float32::matrix_vector_product(out.elements, const_cast<float*>(A.elements), x.elements);
    return out;
}
template <>
auto linalg::operator* <double>(const Matrix3<double>& A, const Vector3<double>& x) -> Vector3<double> {
    auto out = Vector3<double>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float64::matrix_vector_product(out.elements, const_cast<double*>(A.elements), x.elements);
    return out;
}

/*
    Solving
*/
template <>
auto linalg::operator/ <float>(const Matrix3<float>& A, const Vector3<float>& x) -> Vector3<float> {
    auto out = Vector3<float>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float32::solve(const_cast<float*>(out.elements), const_cast<float*>(A.elements),
                                                      const_cast<float*>(x.elements));
    return out;
}
template <>
auto linalg::operator/ <double>(const Matrix3<double>& A, const Vector3<double>& x) -> Vector3<double> {
    auto out = Vector3<double>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float64::solve(const_cast<double*>(out.elements), const_cast<double*>(A.elements),
                                                      const_cast<double*>(x.elements));
    return out;
}
