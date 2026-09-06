#pragma once
#include <fmt/format.h>

#include <array>
#include <concepts>
#include <linalg/vector.h>
#include <string>
#include <string_view>
#include <utility>

namespace ENGINE_NS::reflection {
    template <typename T>
    concept ReflectedScalar = requires(const T& value, void* ptr, const void* const_ptr) {
        typename Type<T>::Inner;
        { Type<T>::name() } -> std::convertible_to<std::string_view>;
        { Type<T>::as_string(value) } -> std::convertible_to<std::string>;
        { Type<T>::as_human_string(value) } -> std::convertible_to<std::string>;
        { Type<T>::cast_from_ptr(ptr) } -> std::same_as<T&>;
        { Type<T>::cast_from_ptr(const_ptr) } -> std::same_as<const T&>;
    };

    template <ReflectedScalar T>
    struct Type<::linalg::Vector2<T>> : Atom<::linalg::Vector2<T>> {
            using Vector = ::linalg::Vector2<T>;

            static constexpr auto name() -> std::string_view {
                return "Vector2";
            }
            static auto as_string(const Vector& var) -> std::string {
                return fmt::format("[{}, {}]", Type<T>::as_string(var.x), Type<T>::as_string(var.y));
            }
            static auto as_human_string(const Vector& var) -> std::string {
                return fmt::format("Vector2<{}>({}, {})", Type<T>::name(), Type<T>::as_human_string(var.x), Type<T>::as_human_string(var.y));
            }
            static auto static_members() -> decltype(auto) {
                Member members[] = {
                    Member{0, "x", RuntimeType::instance<T>(), [](const void* owner) -> const void* { return &static_cast<const Vector*>(owner)->x; }},
                    Member{0, "y", RuntimeType::instance<T>(), [](const void* owner) -> const void* { return &static_cast<const Vector*>(owner)->y; }},
                };
                return std::to_array(members);
            }

            static auto construct() -> Vector {
                return Vector{};
            }
            static auto construct(T x, T y) -> Vector {
                return Vector{std::move(x), std::move(y)};
            }
            static auto cast(const Vector& arg) -> Vector {
                return arg;
            }
    };

    template <ReflectedScalar T>
    struct Type<::linalg::Vector3<T>> : Atom<::linalg::Vector3<T>> {
            using Vector = ::linalg::Vector3<T>;

            static constexpr auto name() -> std::string_view {
                return "Vector3";
            }
            static auto as_string(const Vector& var) -> std::string {
                return fmt::format("[{}, {}, {}]", Type<T>::as_string(var.x), Type<T>::as_string(var.y), Type<T>::as_string(var.z));
            }
            static auto as_human_string(const Vector& var) -> std::string {
                return fmt::format("Vector3<{}>({}, {}, {})",
                                   Type<T>::name(),
                                   Type<T>::as_human_string(var.x),
                                   Type<T>::as_human_string(var.y),
                                   Type<T>::as_human_string(var.z));
            }
            static auto static_members() -> decltype(auto) {
                Member members[] = {
                    Member{0, "x", RuntimeType::instance<T>(), [](const void* owner) -> const void* { return &static_cast<const Vector*>(owner)->x; }},
                    Member{0, "y", RuntimeType::instance<T>(), [](const void* owner) -> const void* { return &static_cast<const Vector*>(owner)->y; }},
                    Member{0, "z", RuntimeType::instance<T>(), [](const void* owner) -> const void* { return &static_cast<const Vector*>(owner)->z; }},
                };
                return std::to_array(members);
            }

            static auto construct() -> Vector {
                return Vector{};
            }
            static auto construct(T x, T y, T z) -> Vector {
                return Vector{std::move(x), std::move(y), std::move(z)};
            }
            static auto cast(const Vector& arg) -> Vector {
                return arg;
            }
    };

    template <ReflectedScalar T>
    struct Type<::linalg::Vector4<T>> : Atom<::linalg::Vector4<T>> {
            using Vector = ::linalg::Vector4<T>;

            static constexpr auto name() -> std::string_view {
                return "Vector4";
            }
            static auto as_string(const Vector& var) -> std::string {
                return fmt::format("[{}, {}, {}, {}]",
                                   Type<T>::as_string(var.x),
                                   Type<T>::as_string(var.y),
                                   Type<T>::as_string(var.z),
                                   Type<T>::as_string(var.w));
            }
            static auto as_human_string(const Vector& var) -> std::string {
                return fmt::format("Vector4<{}>({}, {}, {}, {})",
                                   Type<T>::name(),
                                   Type<T>::as_human_string(var.x),
                                   Type<T>::as_human_string(var.y),
                                   Type<T>::as_human_string(var.z),
                                   Type<T>::as_human_string(var.w));
            }
            static auto static_members() -> decltype(auto) {
                Member members[] = {
                    Member{0, "x", RuntimeType::instance<T>(), [](const void* owner) -> const void* { return &static_cast<const Vector*>(owner)->x; }},
                    Member{0, "y", RuntimeType::instance<T>(), [](const void* owner) -> const void* { return &static_cast<const Vector*>(owner)->y; }},
                    Member{0, "z", RuntimeType::instance<T>(), [](const void* owner) -> const void* { return &static_cast<const Vector*>(owner)->z; }},
                    Member{0, "w", RuntimeType::instance<T>(), [](const void* owner) -> const void* { return &static_cast<const Vector*>(owner)->w; }},
                };
                return std::to_array(members);
            }

            static auto construct() -> Vector {
                return Vector{};
            }
            static auto construct(T x, T y, T z, T w) -> Vector {
                return Vector{std::move(x), std::move(y), std::move(z), std::move(w)};
            }
            static auto cast(const Vector& arg) -> Vector {
                return arg;
            }
    };
} // namespace ENGINE_NS::reflection
