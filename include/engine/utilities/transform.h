#pragma once
#include "engine/meta_defines.h"
#include "engine/utilities/quaternion.h"

#include <linalg/matrix.h>
#include <linalg/vector.h>

namespace ENGINE_NS {
    class Transform {
        public:
            Transform()                                        = default;
            Transform(const Transform& other)                  = default;
            Transform(Transform&& other)                       = default;
            ~Transform()                                       = default;

            auto operator=(const Transform& rhs) -> Transform& = default;
            auto operator=(Transform&& rhs) -> Transform&      = default;

            Transform(const ::linalg::Matrix4<double>& matrix);

            auto translate(const ::linalg::Vector3<double>& offset) -> Transform&;
            auto scale_by(const ::linalg::Vector3<double>& scale_offset) -> Transform&;
            auto scale_by(double scale_offset) -> Transform&;
            auto rotate(const Quaternion& rotation_offset) -> Transform&;

            auto set_translate(const ::linalg::Vector3<double>& new_position) -> Transform&;
            auto set_scale(const ::linalg::Vector3<double>& new_scale) -> Transform&;
            auto set_scale(double new_scale) -> Transform&;
            auto set_rotation(const Quaternion& new_rotation) -> Transform&;

            auto matrix() -> ::linalg::Matrix4<double>;
            auto matrix() const -> ::linalg::Matrix4<double>;

            friend auto operator*(const Transform& lhs, const Transform& rhs) -> Transform;
            friend auto operator*=(Transform& lhs, const Transform& rhs) -> Transform&;

            const ::linalg::Vector3<double>& position = position_;
            const ::linalg::Vector3<double>& scale    = scale_;
            const Quaternion& rotation                = rotation_;

        private:
            ::linalg::Matrix4<double> matrix_ = ::linalg::Matrix4<double>::identity();

            ::linalg::Vector3<double> position_{};
            ::linalg::Vector3<double> scale_{1.0, 1.0, 1.0};
            Quaternion rotation_{};

            bool dirty = true;
    };
} // namespace ENGINE_NS
