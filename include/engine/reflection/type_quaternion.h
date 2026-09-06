#pragma once
#include "engine/utilities/quaternion.h"

#include <fmt/format.h>

#include <string>
#include <string_view>

namespace ENGINE_NS::reflection {
    template <>
    struct Type<ENGINE_NS::Quaternion> : Atom<ENGINE_NS::Quaternion> {
            using Inner = ENGINE_NS::Quaternion;

            static constexpr auto name() -> std::string_view {
                return "Quaternion";
            }
            static auto as_string(const Inner& var) -> std::string {
                const auto imaginary = var.imaginary();
                return fmt::format("[{}, {}, {}, {}]",
                                   Type<double>::as_string(imaginary.x),
                                   Type<double>::as_string(imaginary.y),
                                   Type<double>::as_string(imaginary.z),
                                   Type<double>::as_string(var.scalar()));
            }
            static auto as_human_string(const Inner& var) -> std::string {
                const auto imaginary = var.imaginary();
                return fmt::format("Quaternion({}, {}, {}, {})",
                                   Type<double>::as_human_string(imaginary.x),
                                   Type<double>::as_human_string(imaginary.y),
                                   Type<double>::as_human_string(imaginary.z),
                                   Type<double>::as_human_string(var.scalar()));
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
