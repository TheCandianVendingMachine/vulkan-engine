#pragma once
#include "engine/meta_defines.h"

#include <linalg/matrix.h>
#include <linalg/vector.h>

namespace ENGINE_NS {
    class Quaternion {
        public:
            Quaternion()                                             = default;
            Quaternion(const Quaternion& other)                      = default;
            Quaternion(Quaternion&& other) noexcept                  = default;
            ~Quaternion()                                            = default;

            auto operator=(const Quaternion& rhs) -> Quaternion&     = default;
            auto operator=(Quaternion&& rhs) noexcept -> Quaternion& = default;

            Quaternion(const ::linalg::Vector4<double>& values);
            Quaternion(const ::linalg::Matrix3<double>& rotation);
            Quaternion(const ::linalg::Matrix4<double>& transformation);

            auto scalar() const -> double;
            auto imaginary() const -> ::linalg::Vector3<double>;

            auto conjugate() const -> Quaternion;
            auto as_rotation_matrix() const -> ::linalg::Matrix3<double>;

            friend auto operator*(const Quaternion& lhs, const Quaternion& rhs) -> Quaternion;
            friend auto operator*(const Quaternion& lhs, const ::linalg::Vector3<double>& rhs) -> ::linalg::Vector3<double>;

            friend auto operator*=(Quaternion& lhs, const Quaternion& rhs) -> Quaternion&;

            static auto from_a_to_b(const ::linalg::Vector3<double>& lhs, const ::linalg::Vector3<double>& rhs) -> Quaternion;

        private:
            ::linalg::Vector4<double> values_ = ::linalg::Vector4<double>{0.0, 0.0, 0.0, 1.0};
    };
} // namespace ENGINE_NS
