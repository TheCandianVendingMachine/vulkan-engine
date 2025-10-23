#pragma once
#include "engine/meta_defines.h"
#include <cstdint>
#include <memory>
#include <string>

namespace ENGINE_NS {
    namespace reflection {
        template <typename T>
        struct Atom {
                static constexpr auto size() -> size_t {
                    return sizeof(T);
                }
                static constexpr auto alignment() -> size_t {
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
                static auto cast_from_ptr(const void* from) -> T&;
        };

        template <typename T>
        T underlying_type_impl(Type<T>);
        template <typename T>
        using underlying_type = decltype(underlying_type_impl(std::declval<T>()));

        template <typename T>
        struct Variable {
                using TypeVar = Type<T>;
                T* inner_     = nullptr;

                inline constexpr auto type_name() const -> const char* {
                    return TypeVar::name();
                }
                inline constexpr auto type_size() const -> size_t {
                    return TypeVar::size();
                }
                inline constexpr auto type_alignment() const -> size_t {
                    return TypeVar::alignment();
                }
                inline auto as_string() const -> std::string {
                    return TypeVar::as_string(*inner_);
                }
                inline auto as_human_string() const -> std::string {
                    return TypeVar::as_human_string(*inner_);
                }
                inline auto read() const -> const T& {
                    return TypeVar::cast_from_ptr(inner_);
                }
                inline auto write() -> T& {
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

namespace ENGINE_NS {
    namespace reflection {
        class RuntimeType {
            public:
                virtual auto to_string(const void* data) const -> std::string       = 0;
                virtual auto to_human_string(const void* data) const -> std::string = 0;
                virtual auto size() const -> size_t                                 = 0;
                virtual auto alignment() const -> size_t                            = 0;
                virtual auto name() const -> const char*                            = 0;

                template <typename T>
                static auto instance() -> std::shared_ptr<RuntimeType>;
        };

        class RuntimeTypeInt8 : public RuntimeType {
            public:
                using TypeVar = Type<std::int8_t>;
                virtual auto to_string(const void* data) const -> std::string override final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto to_human_string(const void* data) const -> std::string override final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto size() const -> size_t override final {
                    return TypeVar::size();
                }
                virtual auto alignment() const -> size_t override final {
                    return TypeVar::alignment();
                }
                virtual auto name() const -> const char* override final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeInt16 : public RuntimeType {
            public:
                using TypeVar = Type<std::int16_t>;
                virtual auto to_string(const void* data) const -> std::string override final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto to_human_string(const void* data) const -> std::string override final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto size() const -> size_t override final {
                    return TypeVar::size();
                }
                virtual auto alignment() const -> size_t override final {
                    return TypeVar::alignment();
                }
                virtual auto name() const -> const char* override final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeInt32 : public RuntimeType {
            public:
                using TypeVar = Type<std::int32_t>;
                virtual auto to_string(const void* data) const -> std::string override final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto to_human_string(const void* data) const -> std::string override final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto size() const -> size_t override final {
                    return TypeVar::size();
                }
                virtual auto alignment() const -> size_t override final {
                    return TypeVar::alignment();
                }
                virtual auto name() const -> const char* override final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeInt64 : public RuntimeType {
            public:
                using TypeVar = Type<std::int64_t>;
                virtual auto to_string(const void* data) const -> std::string override final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto to_human_string(const void* data) const -> std::string override final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto size() const -> size_t override final {
                    return TypeVar::size();
                }
                virtual auto alignment() const -> size_t override final {
                    return TypeVar::alignment();
                }
                virtual auto name() const -> const char* override final {
                    return TypeVar::name();
                }
        };

        class RuntimeTypeUInt8 : public RuntimeType {
            public:
                using TypeVar = Type<std::uint8_t>;
                virtual auto to_string(const void* data) const -> std::string override final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto to_human_string(const void* data) const -> std::string override final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto size() const -> size_t override final {
                    return TypeVar::size();
                }
                virtual auto alignment() const -> size_t override final {
                    return TypeVar::alignment();
                }
                virtual auto name() const -> const char* override final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeUInt16 : public RuntimeType {
            public:
                using TypeVar = Type<std::uint16_t>;
                virtual auto to_string(const void* data) const -> std::string override final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto to_human_string(const void* data) const -> std::string override final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto size() const -> size_t override final {
                    return TypeVar::size();
                }
                virtual auto alignment() const -> size_t override final {
                    return TypeVar::alignment();
                }
                virtual auto name() const -> const char* override final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeUInt32 : public RuntimeType {
            public:
                using TypeVar = Type<std::uint32_t>;
                virtual auto to_string(const void* data) const -> std::string override final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto to_human_string(const void* data) const -> std::string override final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto size() const -> size_t override final {
                    return TypeVar::size();
                }
                virtual auto alignment() const -> size_t override final {
                    return TypeVar::alignment();
                }
                virtual auto name() const -> const char* override final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeUInt64 : public RuntimeType {
            public:
                using TypeVar = Type<std::uint64_t>;
                virtual auto to_string(const void* data) const -> std::string override final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto to_human_string(const void* data) const -> std::string override final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto size() const -> size_t override final {
                    return TypeVar::size();
                }
                virtual auto alignment() const -> size_t override final {
                    return TypeVar::alignment();
                }
                virtual auto name() const -> const char* override final {
                    return TypeVar::name();
                }
        };

        class RuntimeTypeFloat32 : public RuntimeType {
            public:
                using TypeVar = Type<float>;
                virtual auto to_string(const void* data) const -> std::string override final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto to_human_string(const void* data) const -> std::string override final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto size() const -> size_t override final {
                    return TypeVar::size();
                }
                virtual auto alignment() const -> size_t override final {
                    return TypeVar::alignment();
                }
                virtual auto name() const -> const char* override final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeFloat64 : public RuntimeType {
            public:
                using TypeVar = Type<double>;
                virtual auto to_string(const void* data) const -> std::string override final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto to_human_string(const void* data) const -> std::string override final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                virtual auto size() const -> size_t override final {
                    return TypeVar::size();
                }
                virtual auto alignment() const -> size_t override final {
                    return TypeVar::alignment();
                }
                virtual auto name() const -> const char* override final {
                    return TypeVar::name();
                }
        };

        struct RuntimeMember;
        struct Member {
                const std::size_t offset_                    = 0;
                const std::string name                       = "";
                const std::shared_ptr<RuntimeType> type_info = nullptr;

                Member()                                     = default;
                Member(std::size_t offset, std::string name, std::shared_ptr<RuntimeType> type_info);
                Member(const Member& rhs);

                auto into_runtime(const void* owner) const -> RuntimeMember;
                inline auto operator=(const Member& rhs) -> Member& {
                    if (&rhs != this) {
                        const_cast<std::size_t&>(this->offset_)                    = rhs.offset_;
                        const_cast<std::string&>(this->name)                       = rhs.name;
                        const_cast<std::shared_ptr<RuntimeType>&>(this->type_info) = rhs.type_info;
                    }
                    return *this;
                }
        };

        struct RuntimeMember {
                const Member meta{};
                const void* owner_ = nullptr;
                auto to_string() const -> std::string;
                auto to_human_string() const -> std::string;

                inline auto operator=(const RuntimeMember& rhs) -> RuntimeMember& {
                    if (&rhs != this) {
                        const_cast<Member&>(this->meta) = rhs.meta;
                        this->owner_                    = rhs.owner_;
                    }
                    return *this;
                }
        };
    } // namespace reflection
} // namespace ENGINE_NS

#include <array>
#include <type_traits>
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
        offsetof(Underlying, Mname), STR(Mname), ENGINE_NS::reflection::RuntimeType::instance<decltype(Underlying::Mname)>()               \
    }

#define REFLECT_END                                                                                                                        \
    }                                                                                                                                      \
    ;                                                                                                                                      \
    return std::to_array(members);                                                                                                         \
    }                                                                                                                                      \
    inline auto members() const -> decltype(auto) {                                                                                        \
        auto members = static_members();                                                                                                   \
        auto owner   = this;                                                                                                               \
        auto arr     = std::array<ENGINE_NS::reflection::RuntimeMember, members.size()>{};                                                 \
        std::transform(members.cbegin(), members.cend(), arr.begin(),                                                                      \
                       [owner](const ENGINE_NS::reflection::Member& member) -> ENGINE_NS::reflection::RuntimeMember {                      \
                           return member.into_runtime(&owner->base);                                                                       \
                       });                                                                                                                 \
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
