#include <fmt/format.h>

#include <concepts>
#include <cstdint>
#include <string>
#include <utility>

template <>
struct ENGINE_NS::reflection::Type<std::uint8_t> : ENGINE_NS::reflection::Atom<std::uint8_t> {
        using Inner = std::uint8_t;

        static constexpr auto name() -> const char* {
            return "uint8";
        }
        static auto as_string(const Inner& var) -> std::string {
            return fmt::format("{}", var);
        }
        static auto as_human_string(const Inner& var) -> std::string {
            return fmt::format("{}_u8", var);
        }

        static auto construct() -> Inner {
            return Inner();
        }
        template <typename T>
            requires std::convertible_to<T, Inner>
        static auto construct(T&& arg) -> Inner {
            return static_cast<Inner>(std::forward<T>(arg));
        }

        template <typename T>
            requires std::convertible_to<T, Inner>
        static auto cast(const T& arg) -> Inner {
            return static_cast<Inner>(arg);
        }

        static auto cast_from_ptr(void* arg) -> Inner& {
            return *reinterpret_cast<Inner*>(arg);
        }
        static auto cast_from_ptr(const void* arg) -> const Inner& {
            return *reinterpret_cast<const Inner*>(arg);
        }
};

template <>
struct ENGINE_NS::reflection::Type<std::int8_t> : ENGINE_NS::reflection::Atom<std::int8_t> {
        using Inner = std::int8_t;

        static constexpr auto name() -> const char* {
            return "int8";
        }
        static auto as_string(const Inner& var) -> std::string {
            return fmt::format("{}", var);
        }
        static auto as_human_string(const Inner& var) -> std::string {
            return fmt::format("{}_i8", var);
        }

        static auto construct() -> Inner {
            return Inner();
        }
        template <typename T>
            requires std::convertible_to<T, Inner>
        static auto construct(T&& arg) -> Inner {
            return static_cast<Inner>(std::forward<T>(arg));
        }

        template <typename T>
            requires std::convertible_to<T, Inner>
        static auto cast(const T& arg) -> Inner {
            return static_cast<Inner>(arg);
        }

        static auto cast_from_ptr(void* arg) -> Inner& {
            return *reinterpret_cast<Inner*>(arg);
        }
        static auto cast_from_ptr(const void* arg) -> const Inner& {
            return *reinterpret_cast<const Inner*>(arg);
        }
};
