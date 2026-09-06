#pragma once
#include "engine/meta_defines.h"

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

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
                static constexpr auto name() -> std::string_view;
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
        concept HasAtomicReflection = requires(const T& value, void* ptr, const void* const_ptr) {
            typename Type<T>::Inner;
            { Type<T>::name() } -> std::convertible_to<std::string_view>;
            { Type<T>::as_string(value) } -> std::convertible_to<std::string>;
            { Type<T>::as_human_string(value) } -> std::convertible_to<std::string>;
            { Type<T>::cast_from_ptr(ptr) } -> std::same_as<T&>;
            { Type<T>::cast_from_ptr(const_ptr) } -> std::same_as<const T&>;
        };

        template <typename T>
        concept HasNativeStaticReflection = requires {
            typename T::Meta;
            { T::Meta::name } -> std::convertible_to<std::string_view>;
            T::Meta::static_members();
        };

        template <typename T>
        concept HasTypeStaticReflection = requires {
            { Type<T>::name() } -> std::convertible_to<std::string_view>;
            Type<T>::static_members();
        };

        template <typename T>
        concept HasStaticReflection = HasNativeStaticReflection<T> || HasTypeStaticReflection<T>;

        template <typename T>
        T underlying_type_impl(Type<T>);
        template <typename T>
        using underlying_type = decltype(underlying_type_impl(std::declval<T>()));

        template <typename T>
        struct Variable {
                using TypeVar = Type<T>;
                T* inner_     = nullptr;

                [[nodiscard]]
                constexpr auto type_name() const -> std::string_view {
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
#include "engine/reflection/type_quaternion.h"

namespace ENGINE_NS {
    namespace reflection {
        struct RuntimeMember;

        enum class RuntimeTypeKind : std::uint8_t {
            Atomic,
            Object,
        };

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
                virtual auto name() const -> std::string_view = 0;
                [[nodiscard]]
                virtual auto kind() const -> RuntimeTypeKind = 0;
                [[nodiscard]]
                virtual auto is_atomic() const -> bool;
                [[nodiscard]]
                virtual auto is_object() const -> bool;
                [[nodiscard]]
                virtual auto members(const void* data) const -> std::vector<RuntimeMember>;
                [[nodiscard]]
                virtual auto member(const void* data, std::string_view name) const -> std::optional<RuntimeMember>;

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
                auto name() const -> std::string_view final {
                    return TypeVar::name();
                }
                [[nodiscard]]
                auto kind() const -> RuntimeTypeKind final {
                    return RuntimeTypeKind::Atomic;
                }
        };

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
                auto data() const -> const void*;
                [[nodiscard]]
                auto has_members() const -> bool;
                [[nodiscard]]
                auto members() const -> std::vector<RuntimeMember>;
                [[nodiscard]]
                auto get(std::string_view name) const -> std::optional<RuntimeMember>;
                [[nodiscard]]
                auto to_string() const -> std::string;
                [[nodiscard]]
                auto to_human_string() const -> std::string;

                RuntimeMember(const Member& meta, const void* owner);
                RuntimeMember()                         = default;
                RuntimeMember(const RuntimeMember& rhs) = default;
                auto operator=(const RuntimeMember& rhs) -> RuntimeMember& = default;
        };

        template <typename T>
        class RuntimeTypeObject : public RuntimeType {
            public:
                auto to_string(const void* data) const -> std::string final {
                    return format_object_(data, false);
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return format_object_(data, true);
                }
                [[nodiscard]]
                auto size() const -> std::size_t final {
                    return sizeof(T);
                }
                [[nodiscard]]
                auto alignment() const -> std::size_t final {
                    return alignof(T);
                }
                [[nodiscard]]
                auto name() const -> std::string_view final {
                    if constexpr (HasTypeStaticReflection<T>) {
                        return Type<T>::name();
                    } else {
                        return T::Meta::name;
                    }
                }
                [[nodiscard]]
                auto kind() const -> RuntimeTypeKind final {
                    return RuntimeTypeKind::Object;
                }
                [[nodiscard]]
                auto members(const void* data) const -> std::vector<RuntimeMember> final {
                    auto static_members = static_members_();
                    auto runtime_members = std::vector<RuntimeMember>{};
                    runtime_members.reserve(static_members.size());
                    for (const auto& member : static_members) {
                        runtime_members.emplace_back(member.into_runtime(data));
                    }
                    return runtime_members;
                }
                [[nodiscard]]
                auto member(const void* data, std::string_view name) const -> std::optional<RuntimeMember> final {
                    for (auto& runtime_member : members(data)) {
                        if (runtime_member.meta.name == name) {
                            return runtime_member;
                        }
                    }
                    return std::nullopt;
                }

            private:
                static auto static_members_() -> decltype(auto) {
                    if constexpr (HasTypeStaticReflection<T>) {
                        return Type<T>::static_members();
                    } else {
                        return T::Meta::static_members();
                    }
                }

                auto format_object_(const void* data, bool human) const -> std::string {
                    auto output = std::string(name());
                    output += "{";
                    bool first = true;
                    for (const auto& runtime_member : members(data)) {
                        if (!first) {
                            output += ", ";
                        }
                        first = false;
                        output += runtime_member.meta.name;
                        output += ": ";
                        output += human ? runtime_member.to_human_string() : runtime_member.to_string();
                    }
                    output += "}";
                    return output;
                }
        };
    } // namespace reflection
} // namespace ENGINE_NS

#include "engine/reflection/type_vector.h"

namespace ENGINE_NS {
    namespace reflection {
        template <typename T>
        auto RuntimeType::instance() -> std::shared_ptr<RuntimeType> {
            if constexpr (HasStaticReflection<T>) {
                static auto instance = std::make_shared<RuntimeTypeObject<T>>();
                return instance;
            } else if constexpr (HasAtomicReflection<T>) {
                static auto instance = std::make_shared<RuntimeTypeAtomic<T>>();
                return instance;
            } else {
                static_assert(always_false_v<T>, "No RuntimeType registered for reflected type T");
                return nullptr;
            }
        }
    } // namespace reflection
} // namespace ENGINE_NS

#define REFLECT_START(Tbase)                                                                                                               \
    struct Meta {                                                                                                                          \
            using Underlying                         = Tbase;                                                                              \
            static constexpr std::string_view name = STR(Tbase);                                                                           \
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
