#include <fmt/format.h>

#include <concepts>
#include <string>
#include <utility>

template <>
struct ENGINE_NS::reflection::Type<float> : ENGINE_NS::reflection::Atom<float> {
        using Inner = float;

        static constexpr auto name() -> const char* {
            return "float32";
        }
        static auto as_string(const Inner& var) -> std::string {
            return fmt::format("{}", var);
        }
        static auto as_human_string(const Inner& var) -> std::string {
            return fmt::format("{:.6f}", var);
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
struct ENGINE_NS::reflection::Type<double> : ENGINE_NS::reflection::Atom<double> {
        using Inner = double;

        static constexpr auto name() -> const char* {
            return "float64";
        }
        static auto as_string(const Inner& var) -> std::string {
            return fmt::format("{}", var);
        }
        static auto as_human_string(const Inner& var) -> std::string {
            return fmt::format("{:.6f}", var);
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
