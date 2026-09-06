#pragma once
#include "engine/meta_defines.h"
#include "engine/utilities/quaternion.h"
#include "engine/reflection/type.h"

#include <fmt/format.h>

#include <linalg/matrix.h>
#include <linalg/vector.h>

#include <string>
#include <string_view>

namespace ENGINE_NS {
    class Transform {
        public:
            Transform()                       = default;
            Transform(const Transform& other) = default;
            Transform(Transform&& other)      = default;
            ~Transform()                      = default;

            auto operator=(const Transform& rhs) -> Transform&;
            auto operator=(Transform&& rhs) noexcept -> Transform&;

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
            [[nodiscard]]
            auto matrix() const -> ::linalg::Matrix4<double>;

            friend auto operator*(const Transform& lhs, const Transform& rhs) -> Transform;
            friend auto operator*=(Transform& lhs, const Transform& rhs) -> Transform&;

            const ::linalg::Vector3<double>& position = position_;
            const ::linalg::Vector3<double>& scale    = scale_;
            const Quaternion& rotation                = rotation_;

        private:
            ::linalg::Matrix4<double> matrix_ = ::linalg::Matrix4<double>::identity();

            ::linalg::Vector3<double> position_;
            ::linalg::Vector3<double> scale_{1.0, 1.0, 1.0};
            Quaternion rotation_;

            bool dirty_ = true;

        public:
            REFLECT_START(Transform)
            REFLECT_MEMBER(position_), REFLECT_MEMBER(scale_), REFLECT_MEMBER(rotation_)
            REFLECT_END
    };
} // namespace ENGINE_NS

namespace ENGINE_NS::reflection {
    template <>
    struct Type<ENGINE_NS::Transform> : Atom<ENGINE_NS::Transform> {
            using Inner = ENGINE_NS::Transform;

            static constexpr auto name() -> std::string_view {
                return "Transform";
            }
            static auto as_string(const Inner& var) -> std::string {
                return fmt::format("{{position: {}, scale: {}, rotation: {}}}",
                                   Type<::linalg::Vector3<double>>::as_string(var.position),
                                   Type<::linalg::Vector3<double>>::as_string(var.scale),
                                   Type<ENGINE_NS::Quaternion>::as_string(var.rotation));
            }
            static auto as_human_string(const Inner& var) -> std::string {
                return fmt::format("Transform(position: {}, scale: {}, rotation: {})",
                                   Type<::linalg::Vector3<double>>::as_human_string(var.position),
                                   Type<::linalg::Vector3<double>>::as_human_string(var.scale),
                                   Type<ENGINE_NS::Quaternion>::as_human_string(var.rotation));
            }

            static auto construct() -> Inner {
                return Inner{};
            }
            static auto cast(const Inner& arg) -> Inner {
                return arg;
            }
            static auto cast_from_ptr(void* arg) -> Inner& {
                return *reinterpret_cast<Inner*>(arg);
            }
            static auto cast_from_ptr(const void* arg) -> const Inner& {
                return *reinterpret_cast<const Inner*>(arg);
            }
    };
} // namespace ENGINE_NS::reflection
