#include "engine/utilities/transform.h"

#include "engine/linalg/matrix.h"
#include "engine/linalg/vector.h"

#include <linalg/matrix.h>
#include <linalg/vector.h>

auto ENGINE_NS::operator*(const Transform& lhs, const Transform& rhs) -> Transform {
    return Transform(lhs.matrix() * rhs.matrix());
}

auto ENGINE_NS::operator*=(Transform& lhs, const Transform& rhs) -> Transform& {
    if (lhs.dirty) {
        lhs.matrix();
    }
    lhs.matrix_ *= rhs.matrix();
    return lhs;
}

ENGINE_NS::Transform::Transform(const ::linalg::Matrix4<double>& matrix) :
    matrix_(matrix), position_{matrix.r3c1, matrix.r3c2, matrix.r3c3}, scale_{matrix.r1c1, matrix.r2c2, matrix.r3c3}, rotation_{matrix} {
}

auto ENGINE_NS::Transform::translate(const ::linalg::Vector3<double>& offset) -> Transform& {
    position_ += offset;
    dirty = true;
    return *this;
}

auto ENGINE_NS::Transform::scale_by(const ::linalg::Vector3<double>& scale_offset) -> Transform& {
    scale_ += scale_offset;
    dirty = true;
    return *this;
}

auto ENGINE_NS::Transform::scale_by(double scale_offset) -> Transform& {
    scale_ += {scale_offset, scale_offset, scale_offset};
    dirty = true;
    return *this;
}

auto ENGINE_NS::Transform::rotate(const Quaternion& rotation_offset) -> Transform& {
    rotation_ *= rotation_offset;
    dirty = true;
    return *this;
}

auto ENGINE_NS::Transform::set_translate(const ::linalg::Vector3<double>& new_position) -> Transform& {
    position_ = new_position;
    dirty     = true;
    return *this;
}

auto ENGINE_NS::Transform::set_scale(const ::linalg::Vector3<double>& new_scale) -> Transform& {
    scale_ = new_scale;
    dirty  = true;
    return *this;
}

auto ENGINE_NS::Transform::set_scale(double new_scale) -> Transform& {
    scale_ = {new_scale, new_scale, new_scale};
    dirty  = true;
    return *this;
}

auto ENGINE_NS::Transform::set_rotation(const Quaternion& new_rotation) -> Transform& {
    rotation_ = new_rotation;
    dirty     = true;
    return *this;
}

auto ENGINE_NS::Transform::matrix() -> ::linalg::Matrix4<double> {
    if (dirty) {
        matrix_ = static_cast<const Transform*>(this)->matrix();
        dirty   = false;
    }
    return matrix_;
}

auto ENGINE_NS::Transform::matrix() const -> ::linalg::Matrix4<double> {
    if (!dirty) {
        return matrix_;
    }
    auto our_rotation = rotation_.as_rotation_matrix();

    ::linalg::Vector3<double> row0 = *reinterpret_cast<::linalg::Vector3<double>*>(&our_rotation.elements[0]);
    ::linalg::Vector3<double> row1 = *reinterpret_cast<::linalg::Vector3<double>*>(&our_rotation.elements[3]);
    ::linalg::Vector3<double> row2 = *reinterpret_cast<::linalg::Vector3<double>*>(&our_rotation.elements[6]);

    return ::linalg::Matrix4<double>{
      {scale_.x * row0.x, row0.y,            row0.z,            0.0},
      {row1.x,            scale_.y * row1.y, row1.z,            0.0},
      {row2.x,            row2.y,            scale_.z * row2.z, 0.0},
      {position_.x,       position_.y,       position_.z,       1.0},
    };
}
