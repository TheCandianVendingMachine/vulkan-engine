#include <fmt/format.h>

#include <string>
#include <utility>

template <>
struct ENGINE_NS::reflection::Type<std::string> : ENGINE_NS::reflection::Atom<std::string> {
        using Inner = std::string;

        static constexpr auto name() -> const char* {
            return "string";
        }
        static auto as_string(const Inner& var) -> std::string {
            return var;
        }
        static auto as_human_string(const Inner& var) -> std::string {
            return fmt::format("\"{}\"", var);
        }

        static auto construct() -> Inner {
            return Inner();
        }
        static auto construct(std::string arg) -> Inner {
            return arg;
        }

        static auto cast(const std::string& arg) -> Inner {
            return arg;
        }

        static auto cast_from_ptr(void* arg) -> Inner& {
            return *reinterpret_cast<Inner*>(arg);
        }
        static auto cast_from_ptr(const void* arg) -> const Inner& {
            return *reinterpret_cast<const Inner*>(arg);
        }
};
