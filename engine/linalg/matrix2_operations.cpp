#include "engine/linalg/matrix.h"
#include "engine/linalg/vector.h"
#include "engine/meta_defines.h"

using namespace linalg;

/*
    Addition
*/
template <>
auto linalg::operator+ <float>(const Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float> {
    auto row1 = Vector2<float>::zero();
    auto row2 = Vector2<float>::zero();

    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row1.elements, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row2.elements, 1.f, &A.r2c1, &B.r2c1);

    return Matrix2<float>(row1, row2);
}
template <>
auto linalg::operator+ <double>(const Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double> {
    auto row1 = Vector2<double>::zero();
    auto row2 = Vector2<double>::zero();

    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row1.elements, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row2.elements, 1.f, &A.r2c1, &B.r2c1);

    return Matrix2<double>(row1, row2);
}

template <>
auto linalg::operator+= <float>(Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float>& {
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r1c1, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r2c1, 1.f, &A.r2c1, &B.r2c1);

    return A;
}
template <>
auto linalg::operator+= <double>(Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double>& {
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r1c1, 1.f, &A.r1c1, &B.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r2c1, 1.f, &A.r2c1, &B.r2c1);

    return A;
}

/*
    Subtraction
*/
template <>
auto linalg::operator- <float>(const Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float> {
    auto row1 = Vector2<float>::zero();
    auto row2 = Vector2<float>::zero();

    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row1.elements, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(row2.elements, -1.f, &B.r2c1, &A.r2c1);

    return Matrix2<float>(row1, row2);
}
template <>
auto linalg::operator- <double>(const Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double> {
    auto row1 = Vector2<double>::zero();
    auto row2 = Vector2<double>::zero();

    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row1.elements, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(row2.elements, -1.f, &B.r2c1, &A.r2c1);

    return Matrix2<double>(row1, row2);
}

template <>
auto linalg::operator-= <float>(Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float>& {
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r1c1, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float32::axpy(&A.r2c1, -1.f, &B.r2c1, &A.r2c1);

    return A;
}
template <>
auto linalg::operator-= <double>(Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double>& {
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r1c1, -1.f, &B.r1c1, &A.r1c1);
    ENGINE_NS::linalg::blas1::vector2::float64::axpy(&A.r2c1, -1.f, &B.r2c1, &A.r2c1);

    return A;
}

/*
    Matrix Multiplication
*/
template <>
auto linalg::operator* <float>(const Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float> {
    auto col1 = Vector2<float>(B.c1r1, B.c1r2);
    auto col2 = Vector2<float>(B.c2r1, B.c2r2);

    auto r1c1 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r1, col1.elements);
    auto r1c2 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r1, col2.elements);

    auto r2c1 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r2, col1.elements);
    auto r2c2 = ENGINE_NS::linalg::blas1::vector2::float32::dot(&A.c1r2, col2.elements);

    return Matrix2<float>(r1c1, r1c2, r2c1, r2c2);
}
template <>
auto linalg::operator* <double>(const Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double> {
    auto col1 = Vector2<double>(B.c1r1, B.c1r2);
    auto col2 = Vector2<double>(B.c2r1, B.c2r2);

    auto r1c1 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r1, col1.elements);
    auto r1c2 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r1, col2.elements);

    auto r2c1 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r2, col1.elements);
    auto r2c2 = ENGINE_NS::linalg::blas1::vector2::float64::dot(&A.c1r2, col2.elements);

    return Matrix2<double>(r1c1, r1c2, r2c1, r2c2);
}

template <>
auto linalg::operator*= <float>(Matrix2<float>& A, const Matrix2<float>& B) -> Matrix2<float>& {
    A = A * B;
    return A;
}
template <>
auto linalg::operator*= <double>(Matrix2<double>& A, const Matrix2<double>& B) -> Matrix2<double>& {
    A = A * B;
    return A;
}

/*
    Vector Multiplication
*/
template <>
auto linalg::operator* <float>(const Matrix2<float>& A, const Vector2<float>& x) -> Vector2<float> {
    auto out = Vector2<float>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float32::matrix_vector_product(out.elements, const_cast<float*>(A.elements), x.elements);
    return out;
}
template <>
auto linalg::operator* <double>(const Matrix2<double>& A, const Vector2<double>& x) -> Vector2<double> {
    auto out = Vector2<double>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float64::matrix_vector_product(out.elements, const_cast<double*>(A.elements), x.elements);
    return out;
}

/*
    Solving
*/
template <>
auto linalg::operator/ <float>(const Matrix2<float>& A, const Vector2<float>& x) -> Vector2<float> {
    auto out = Vector2<float>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float32::solve(const_cast<float*>(out.elements), const_cast<float*>(A.elements),
                                                      const_cast<float*>(x.elements));
    return out;
}
template <>
auto linalg::operator/ <double>(const Matrix2<double>& A, const Vector2<double>& x) -> Vector2<double> {
    auto out = Vector2<double>::zero();
    ENGINE_NS::linalg::blas2::matrix2::float64::solve(const_cast<double*>(out.elements), const_cast<double*>(A.elements),
                                                      const_cast<double*>(x.elements));
    return out;
}
