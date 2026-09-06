#pragma once
#include "engine/meta_defines.h"

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ENGINE_NS {
    namespace reflection {
        template <typename>
        inline constexpr bool always_false_v = false;

        template <typename T>
        struct Atom {
                static constexpr auto size() -> std::size_t {
                    return sizeof(T);
                }
                static constexpr auto alignment() -> std::size_t {
                    return alignof(T);
                }
        };

        template <typename T>
        struct Type : Atom<T> {
                static constexpr auto name() -> const char*;
                static auto as_string(const T& var) -> std::string;
                static auto as_human_string(const T& var) -> std::string {
                    return Type<T>::as_string(var);
                }

                static auto construct() -> T;
                template <typename G>
                static auto construct(G&& arg) -> T;
                template <typename... TArgs>
                static auto construct(TArgs&&... args) -> T;

                template <typename G>
                static auto cast(const G& from) -> T;
                static auto cast_from_ptr(void* from) -> T&;
                static auto cast_from_ptr(const void* from) -> const T&;
        };

        template <typename T>
        T underlying_type_impl(Type<T>);
        template <typename T>
        using underlying_type = decltype(underlying_type_impl(std::declval<T>()));

        template <typename T>
        struct Variable {
                using TypeVar = Type<T>;
                T* inner_     = nullptr;

                [[nodiscard]]
                constexpr auto type_name() const -> const char* {
                    return TypeVar::name();
                }
                [[nodiscard]]
                constexpr auto type_size() const -> std::size_t {
                    return TypeVar::size();
                }
                [[nodiscard]]
                constexpr auto type_alignment() const -> std::size_t {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto as_string() const -> std::string {
                    return TypeVar::as_string(*inner_);
                }
                [[nodiscard]]
                auto as_human_string() const -> std::string {
                    return TypeVar::as_human_string(*inner_);
                }
                auto read() const -> const T& {
                    return TypeVar::cast_from_ptr(inner_);
                }
                auto write() -> T& {
                    return TypeVar::cast_from_ptr(inner_);
                }
        };
    } // namespace reflection
} // namespace ENGINE_NS

#include "engine/reflection/type_float.h"
#include "engine/reflection/type_int16.h"
#include "engine/reflection/type_int32.h"
#include "engine/reflection/type_int64.h"
#include "engine/reflection/type_int8.h"
#include "engine/reflection/type_string.h"
#include "engine/reflection/type_vector.h"

namespace ENGINE_NS {
    namespace reflection {
        class RuntimeType {
            public:
                virtual ~RuntimeType()                                              = default;
                virtual auto to_string(const void* data) const -> std::string       = 0;
                virtual auto to_human_string(const void* data) const -> std::string = 0;
                [[nodiscard]]
                virtual auto size() const -> std::size_t = 0;
                [[nodiscard]]
                virtual auto alignment() const -> std::size_t = 0;
                [[nodiscard]]
                virtual auto name() const -> const char* = 0;

                template <typename T>
                static auto instance() -> std::shared_ptr<RuntimeType>;
        };

        template <typename T>
        class RuntimeTypeAtomic : public RuntimeType {
            public:
                using TypeVar = Type<T>;
                using Inner   = T;

                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const Inner*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const Inner*>(data));
                }
                [[nodiscard]]
                auto size() const -> std::size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> std::size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };

        template <typename T>
        auto RuntimeType::instance() -> std::shared_ptr<RuntimeType> {
            if constexpr (requires { Type<T>::name(); }) {
                static auto instance = std::make_shared<RuntimeTypeAtomic<T>>();
                return instance;
            } else {
                static_assert(always_false_v<T>, "No RuntimeType registered for reflected type T");
                return nullptr;
            }
        }

        struct RuntimeMember;
        struct Member {
                using Accessor = std::function<const void*(const void*)>;

                std::size_t offset_ = 0;
                std::string name;
                std::shared_ptr<RuntimeType> type_info = nullptr;
                Accessor accessor{};

                Member() = default;
                Member(std::size_t offset, std::string name, std::shared_ptr<RuntimeType> type_info, Accessor accessor);
                Member(const Member& rhs);

                auto data(const void* owner) const -> const void*;
                auto into_runtime(const void* owner) const -> RuntimeMember;
                auto operator=(const Member& rhs) -> Member& = default;
        };

        struct RuntimeMember {
                Member meta;
                const void* owner_ = nullptr;
                [[nodiscard]]
                auto to_string() const -> std::string;
                [[nodiscard]]
                auto to_human_string() const -> std::string;

                RuntimeMember(const Member& meta, const void* owner);
                RuntimeMember()                         = default;
                RuntimeMember(const RuntimeMember& rhs) = default;
                auto operator=(const RuntimeMember& rhs) -> RuntimeMember& = default;
        };
    } // namespace reflection
} // namespace ENGINE_NS

#define REFLECT_START(Tbase)                                                                                                               \
    struct Meta {                                                                                                                          \
            using Underlying                  = Tbase;                                                                                     \
            static constexpr const char* name = STR(Tbase);                                                                                \
            Underlying& base;                                                                                                              \
            explicit inline Meta(Underlying& from) : base(from) {                                                                          \
            }                                                                                                                              \
            static inline auto static_members() -> decltype(auto) {                                                                        \
                using Member     = ENGINE_NS::reflection::Member;                                                                          \
                Member members[] = {
#define REFLECT_MEMBER(Mname)                                                                                                              \
    Member {                                                                                                                               \
        0,                                                                                                                                 \
            STR(Mname),                                                                                                                    \
            ENGINE_NS::reflection::RuntimeType::instance<                                                                                  \
                std::remove_cvref_t<decltype(std::declval<Underlying&>().Mname)>>(),                                                       \
            [](const void* owner) -> const void* {                                                                                         \
                return &static_cast<const Underlying*>(owner)->Mname;                                                                      \
            }                                                                                                                              \
    }

#define REFLECT_END                                                                                                                        \
    }                                                                                                                                      \
    ;                                                                                                                                      \
    return std::to_array(members);                                                                                                         \
    }                                                                                                                                      \
    [[nodiscard]]                                                                                                                          \
    auto members() const -> decltype(auto) {                                                                                               \
        auto members      = static_members();                                                                                              \
        const auto* owner = this;                                                                                                          \
        auto arr          = std::array<ENGINE_NS::reflection::RuntimeMember, members.size()>{};                                            \
        std::ranges::transform(members,                                                                                                    \
                               arr.begin(),                                                                                                \
                               [owner](const ENGINE_NS::reflection::Member& member) -> ENGINE_NS::reflection::RuntimeMember {              \
                                   return member.into_runtime(&owner->base);                                                               \
                               });                                                                                                         \
        return arr;                                                                                                                        \
    }                                                                                                                                      \
    auto get(std::string_view variable) const -> ENGINE_NS::reflection::RuntimeMember {                                                    \
        for (auto& member : members()) {                                                                                                   \
            if (member.meta.name == variable) {                                                                                            \
                return member;                                                                                                             \
            }                                                                                                                              \
        }                                                                                                                                  \
        std::unreachable();                                                                                                                \
    }                                                                                                                                      \
    }                                                                                                                                      \
    ;                                                                                                                                      \
    auto meta() -> Meta {                                                                                                                  \
        return Meta(*this);                                                                                                                \
    }
