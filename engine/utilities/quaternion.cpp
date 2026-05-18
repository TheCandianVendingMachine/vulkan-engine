#include "engine/utilities/quaternion.h"

#include "engine/linalg/vector_operations.h"

#include <linalg/matrix.h>
#include <linalg/vector.h>

#include <cmath>

ENGINE_NS::Quaternion::Quaternion(const ::linalg::Vector4<double>& values) : values_(values) {
}

ENGINE_NS::Quaternion::Quaternion(const ::linalg::Matrix3<double>& rotation) {
    /*
        Algorithm taken from
       https://github.com/blender/blender/blob/756538b4a117cb51a15e848fa6170143b6aafcd8/source/blender/blenlib/intern/math_rotation.c#L272
        (GPL)
    */
    if (rotation.m22 < 0.0) {
        if (rotation.m00 > rotation.m11) {
            auto trace = 1.0 + rotation.m00 - rotation.m11 - rotation.m22;
            auto s     = 2.0 * std::sqrt(trace);
            if (rotation.m12 < rotation.m21) {
                s = -s;
            }
            auto s_inv = 1.0 / s;
            values_    = {(rotation.m12 - rotation.m21) * s_inv,
                          0.25 * s,
                          (rotation.m01 + rotation.m10) * s_inv,
                          (rotation.m20 + rotation.m02) * s_inv};
            if (trace == 1.0 && (values_.x == 0.0 && values_.z == 0.0 && values_.w == 0.0)) [[unlikely]] {
                values_.y = 1.0;
            }
        } else {
            auto trace = 1.0 - rotation.m00 - rotation.m11 + rotation.m22;
            auto s     = 2.0 * std::sqrt(trace);
            if (rotation.m20 < rotation.m02) {
                s = -s;
            }
            auto s_inv = 1.0 / s;
            values_    = {(rotation.m20 - rotation.m02) * s_inv,
                          (rotation.m01 + rotation.m10) * s_inv,
                          0.25 * s,
                          (rotation.m12 + rotation.m21) * s_inv};
            if (trace == 1.0 && (values_.x == 0.0 && values_.y == 0.0 && values_.w == 0.0)) [[unlikely]] {
                values_.z = 1.0;
            }
        }
    } else {
        if (rotation.m00 < -rotation.m11) {
            auto trace = 1.0 - rotation.m00 + rotation.m11 + rotation.m22;
            auto s     = 2.0 * std::sqrt(trace);
            if (rotation.m01 < rotation.m10) {
                s = -s;
            }
            auto s_inv = 1.0 / s;
            values_    = {(rotation.m01 - rotation.m10) * s_inv,
                          (rotation.m20 + rotation.m02) * s_inv,
                          (rotation.m12 + rotation.m21) * s_inv,
                          0.25 * s};
            if (trace == 1.0 && (values_.x == 0.0 && values_.y == 0.0 && values_.z == 0.0)) [[unlikely]] {
                values_.w = 1.0;
            }
        } else {
            auto trace = 1.0 + rotation.m00 + rotation.m11 + rotation.m22;
            auto s     = 2.0 * std::sqrt(trace);
            auto s_inv = 1.0 / s;
            values_    = {0.25 * s,
                          (rotation.m12 - rotation.m21) * s_inv,
                          (rotation.m20 - rotation.m02) * s_inv,
                          (rotation.m01 - rotation.m10) * s_inv};
            if (trace == 1.0 && (values_.y == 0.0 && values_.z == 0.0 && values_.w == 0.0)) [[unlikely]] {
                values_.x = 1.0;
            }
        }
    }
}

ENGINE_NS::Quaternion::Quaternion(const ::linalg::Matrix4<double>& transformation) {
    /*
        Algorithm taken from
       https://github.com/blender/blender/blob/756538b4a117cb51a15e848fa6170143b6aafcd8/source/blender/blenlib/intern/math_transformation.c#L272
        (GPL)
    */
    if (transformation.m22 < 0.0) {
        if (transformation.m00 > transformation.m11) {
            auto trace = 1.0 + transformation.m00 - transformation.m11 - transformation.m22;
            auto s     = 2.0 * std::sqrt(trace);
            if (transformation.m12 < transformation.m21) {
                s = -s;
            }
            auto s_inv = 1.0 / s;
            values_    = {(transformation.m12 - transformation.m21) * s_inv,
                          0.25 * s,
                          (transformation.m01 + transformation.m10) * s_inv,
                          (transformation.m20 + transformation.m02) * s_inv};
            if (trace == 1.0 && (values_.x == 0.0 && values_.z == 0.0 && values_.w == 0.0)) [[unlikely]] {
                values_.y = 1.0;
            }
        } else {
            auto trace = 1.0 - transformation.m00 - transformation.m11 + transformation.m22;
            auto s     = 2.0 * std::sqrt(trace);
            if (transformation.m20 < transformation.m02) {
                s = -s;
            }
            auto s_inv = 1.0 / s;
            values_    = {(transformation.m20 - transformation.m02) * s_inv,
                          (transformation.m01 + transformation.m10) * s_inv,
                          0.25 * s,
                          (transformation.m12 + transformation.m21) * s_inv};
            if (trace == 1.0 && (values_.x == 0.0 && values_.y == 0.0 && values_.w == 0.0)) [[unlikely]] {
                values_.z = 1.0;
            }
        }
    } else {
        if (transformation.m00 < -transformation.m11) {
            auto trace = 1.0 - transformation.m00 + transformation.m11 + transformation.m22;
            auto s     = 2.0 * std::sqrt(trace);
            if (transformation.m01 < transformation.m10) {
                s = -s;
            }
            auto s_inv = 1.0 / s;
            values_    = {(transformation.m01 - transformation.m10) * s_inv,
                          (transformation.m20 + transformation.m02) * s_inv,
                          (transformation.m12 + transformation.m21) * s_inv,
                          0.25 * s};
            if (trace == 1.0 && (values_.x == 0.0 && values_.y == 0.0 && values_.z == 0.0)) [[unlikely]] {
                values_.w = 1.0;
            }
        } else {
            auto trace = 1.0 + transformation.m00 + transformation.m11 + transformation.m22;
            auto s     = 2.0 * std::sqrt(trace);
            auto s_inv = 1.0 / s;
            values_    = {0.25 * s,
                          (transformation.m12 - transformation.m21) * s_inv,
                          (transformation.m20 - transformation.m02) * s_inv,
                          (transformation.m01 - transformation.m10) * s_inv};
            if (trace == 1.0 && (values_.y == 0.0 && values_.z == 0.0 && values_.w == 0.0)) [[unlikely]] {
                values_.x = 1.0;
            }
        }
    }
}

auto ENGINE_NS::Quaternion::scalar() const -> double {
    return values_.w;
}

auto ENGINE_NS::Quaternion::imaginary() const -> ::linalg::Vector3<double> {
    return ::linalg::Vector3<double>{values_.x, values_.y, values_.z};
}

auto ENGINE_NS::Quaternion::conjugate() const -> Quaternion {
    return Quaternion(::linalg::Vector4<double>{-values_.x, -values_.y, -values_.z, values_.w});
}

auto ENGINE_NS::Quaternion::as_rotation_matrix() const -> ::linalg::Matrix3<double> {
    return ::linalg::Matrix3<double>{
      {1.0 - 2.0 * values_.y * values_.y - 2.0 * values_.z * values_.z,
       2.0 * values_.x * values_.y - 2.0 * values_.w * values_.z,
       2.0 * values_.x * values_.z + 2.0 * values_.w * values_.y      },
      {2.0 * values_.x * values_.y + 2.0 * values_.w * values_.y,
       1.0 - 2.0 * values_.x * values_.x - 2.0 * values_.z * values_.z,
       2.0 * values_.y * values_.z - 2.0 * values_.w * values_.x      },
      {2.0 * values_.x * values_.z - 2.0 * values_.w * values_.y,
       2.0 * values_.y * values_.z + 2.0 * values_.w * values_.x,
       1.0 - 2.0 * values_.x * values_.x - 2.0 * values_.y * values_.y}
    };
}

auto ENGINE_NS::Quaternion::from_a_to_b(const ::linalg::Vector3<double>& lhs, const ::linalg::Vector3<double>& rhs) -> Quaternion {
    auto k_cos_theta = ::linalg::dot(lhs, rhs);
    auto k           = std::sqrt(::linalg::magnitude_sqr(lhs) * ::linalg::magnitude_sqr(rhs));

    auto sum = 0.0;
    if (k_cos_theta / k != -1) {
        sum = k_cos_theta + k;
    }
    auto cross      = ::linalg::cross(lhs, rhs);
    auto quaternion = Quaternion(::linalg::Vector4<double>{cross.x, cross.y, cross.z, sum});
    ::linalg::normalize_inplace(quaternion.values_);

    return quaternion;
}

auto ENGINE_NS::operator*(const Quaternion& lhs, const Quaternion& rhs) -> Quaternion {
    auto lhs_imaginary = lhs.imaginary();
    auto rhs_imaginary = rhs.imaginary();

    auto lhs_scalar = lhs.scalar();
    auto rhs_scalar = rhs.scalar();

    auto scalar    = lhs_scalar * rhs_scalar - (::linalg::dot(lhs_imaginary, rhs_imaginary));
    auto imaginary = rhs_imaginary * lhs_scalar + lhs_imaginary * rhs_scalar + ::linalg::cross(lhs_imaginary, rhs_imaginary);

    return Quaternion(::linalg::Vector4<double>{imaginary.x, imaginary.y, imaginary.z, scalar});
}

auto ENGINE_NS::operator*(const Quaternion& lhs, const ::linalg::Vector3<double>& rhs) -> ::linalg::Vector3<double> {
    auto real      = lhs.scalar();
    auto imaginary = lhs.imaginary();
    return rhs + 2.0 * ::linalg::cross(imaginary, ::linalg::cross(imaginary, rhs) + rhs * real);
}

auto ENGINE_NS::operator*=(Quaternion& lhs, const Quaternion& rhs) -> Quaternion& {
    lhs = lhs * rhs;
    return lhs;
}
