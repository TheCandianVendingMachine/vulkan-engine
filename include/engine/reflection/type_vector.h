#pragma once
#include <fmt/format.h>

#include <concepts>
#include <linalg/vector.h>
#include <string>
#include <string_view>
#include <utility>

namespace ENGINE_NS::reflection {
    template <typename T>
    concept ReflectedScalar = requires(const T& value, void* ptr, const void* const_ptr) {
        { Type<T>::name() } -> std::convertible_to<std::string_view>;
        { Type<T>::as_string(value) } -> std::convertible_to<std::string>;
        { Type<T>::as_human_string(value) } -> std::convertible_to<std::string>;
        { Type<T>::cast_from_ptr(ptr) } -> std::same_as<T&>;
        { Type<T>::cast_from_ptr(const_ptr) } -> std::same_as<const T&>;
    };

    template <ReflectedScalar T>
    struct Type<::linalg::Vector2<T>> : Atom<::linalg::Vector2<T>> {
            using Inner = ::linalg::Vector2<T>;

            static constexpr auto name() -> std::string_view {
                return "Vector2";
            }
            static auto as_string(const Inner& var) -> std::string {
                return fmt::format("[{}, {}]", Type<T>::as_string(var.x), Type<T>::as_string(var.y));
            }
            static auto as_human_string(const Inner& var) -> std::string {
                return fmt::format("Vector2<{}>({}, {})", Type<T>::name(), Type<T>::as_human_string(var.x), Type<T>::as_human_string(var.y));
            }

            static auto construct() -> Inner {
                return Inner{};
            }
            static auto construct(T x, T y) -> Inner {
                return Inner{std::move(x), std::move(y)};
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

    template <ReflectedScalar T>
    struct Type<::linalg::Vector3<T>> : Atom<::linalg::Vector3<T>> {
            using Inner = ::linalg::Vector3<T>;

            static constexpr auto name() -> std::string_view {
                return "Vector3";
            }
            static auto as_string(const Inner& var) -> std::string {
                return fmt::format("[{}, {}, {}]", Type<T>::as_string(var.x), Type<T>::as_string(var.y), Type<T>::as_string(var.z));
            }
            static auto as_human_string(const Inner& var) -> std::string {
                return fmt::format("Vector3<{}>({}, {}, {})",
                                   Type<T>::name(),
                                   Type<T>::as_human_string(var.x),
                                   Type<T>::as_human_string(var.y),
                                   Type<T>::as_human_string(var.z));
            }

            static auto construct() -> Inner {
                return Inner{};
            }
            static auto construct(T x, T y, T z) -> Inner {
                return Inner{std::move(x), std::move(y), std::move(z)};
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

    template <ReflectedScalar T>
    struct Type<::linalg::Vector4<T>> : Atom<::linalg::Vector4<T>> {
            using Inner = ::linalg::Vector4<T>;

            static constexpr auto name() -> std::string_view {
                return "Vector4";
            }
            static auto as_string(const Inner& var) -> std::string {
                return fmt::format("[{}, {}, {}, {}]",
                                   Type<T>::as_string(var.x),
                                   Type<T>::as_string(var.y),
                                   Type<T>::as_string(var.z),
                                   Type<T>::as_string(var.w));
            }
            static auto as_human_string(const Inner& var) -> std::string {
                return fmt::format("Vector4<{}>({}, {}, {}, {})",
                                   Type<T>::name(),
                                   Type<T>::as_human_string(var.x),
                                   Type<T>::as_human_string(var.y),
                                   Type<T>::as_human_string(var.z),
                                   Type<T>::as_human_string(var.w));
            }

            static auto construct() -> Inner {
                return Inner{};
            }
            static auto construct(T x, T y, T z, T w) -> Inner {
                return Inner{std::move(x), std::move(y), std::move(z), std::move(w)};
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
