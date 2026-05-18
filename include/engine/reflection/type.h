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

                [[nodiscard]]
                constexpr auto type_name() const -> const char* {
                    return TypeVar::name();
                }
                [[nodiscard]]
                constexpr auto type_size() const -> size_t {
                    return TypeVar::size();
                }
                [[nodiscard]]
                constexpr auto type_alignment() const -> size_t {
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
                virtual auto size() const -> size_t = 0;
                [[nodiscard]]
                virtual auto alignment() const -> size_t = 0;
                [[nodiscard]]
                virtual auto name() const -> const char* = 0;

                template <typename T>
                static auto instance() -> std::shared_ptr<RuntimeType>;
        };

        class RuntimeTypeInt8 : public RuntimeType {
            public:
                using TypeVar = Type<std::int8_t>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeInt16 : public RuntimeType {
            public:
                using TypeVar = Type<std::int16_t>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeInt32 : public RuntimeType {
            public:
                using TypeVar = Type<std::int32_t>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeInt64 : public RuntimeType {
            public:
                using TypeVar = Type<std::int64_t>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };

        class RuntimeTypeUInt8 : public RuntimeType {
            public:
                using TypeVar = Type<std::uint8_t>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeUInt16 : public RuntimeType {
            public:
                using TypeVar = Type<std::uint16_t>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeUInt32 : public RuntimeType {
            public:
                using TypeVar = Type<std::uint32_t>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeUInt64 : public RuntimeType {
            public:
                using TypeVar = Type<std::uint64_t>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };

        class RuntimeTypeFloat32 : public RuntimeType {
            public:
                using TypeVar = Type<float>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };
        class RuntimeTypeFloat64 : public RuntimeType {
            public:
                using TypeVar = Type<double>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };

        class RuntimeTypeString : public RuntimeType {
            public:
                using TypeVar = Type<std::string>;
                auto to_string(const void* data) const -> std::string final {
                    return TypeVar::as_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                auto to_human_string(const void* data) const -> std::string final {
                    return TypeVar::as_human_string(*static_cast<const underlying_type<TypeVar>*>(data));
                }
                [[nodiscard]]
                auto size() const -> size_t final {
                    return TypeVar::size();
                }
                [[nodiscard]]
                auto alignment() const -> size_t final {
                    return TypeVar::alignment();
                }
                [[nodiscard]]
                auto name() const -> const char* final {
                    return TypeVar::name();
                }
        };

        struct RuntimeMember;
        struct Member {
                const std::size_t offset_ = 0;
                const std::string name;
                const std::shared_ptr<RuntimeType> type_info = nullptr;

                Member() = default;
                Member(std::size_t offset, std::string name, std::shared_ptr<RuntimeType> type_info);
                Member(const Member& rhs);

                auto into_runtime(const void* owner) const -> RuntimeMember;
                auto operator=(const Member& rhs) -> Member& {
                    if (&rhs != this) {
                        const_cast<std::size_t&>(this->offset_)                    = rhs.offset_;
                        const_cast<std::string&>(this->name)                       = rhs.name;
                        const_cast<std::shared_ptr<RuntimeType>&>(this->type_info) = rhs.type_info;
                    }
                    return *this;
                }
        };

        struct RuntimeMember {
                const Member meta;
                const void* owner_ = nullptr;
                [[nodiscard]]
                auto to_string() const -> std::string;
                [[nodiscard]]
                auto to_human_string() const -> std::string;

                RuntimeMember(const Member& meta, const void* owner);
                RuntimeMember()                         = default;
                RuntimeMember(const RuntimeMember& rhs) = default;
                auto operator=(const RuntimeMember& rhs) -> RuntimeMember& {
                    if (&rhs != this) {
                        const_cast<Member&>(this->meta) = rhs.meta;
                        this->owner_                    = rhs.owner_;
                    }
                    return *this;
                }
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
        [] {                                                                                                                               \
            /* aligned storage avoids constructing the object */                                                                           \
            alignas(Underlying) std::byte buf[sizeof(Underlying)]{};                                                                       \
            auto* p = reinterpret_cast<Underlying*>(buf);                                                                                  \
            return static_cast<std::size_t>(reinterpret_cast<std::byte*>(&p->Mname) - reinterpret_cast<std::byte*>(p));                    \
        }(),                                                                                                                               \
            STR(Mname), ENGINE_NS::reflection::RuntimeType::instance<decltype(Underlying::Mname)>()                                        \
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
                               [owner](const engine ::reflection ::Member& member) -> engine ::reflection ::RuntimeMember {                \
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
