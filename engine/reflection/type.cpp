#include "engine/reflection/type.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace ENGINE_NS {
    namespace reflection {
        auto RuntimeType::is_atomic() const -> bool {
            return kind() == RuntimeTypeKind::Atomic;
        }

        auto RuntimeType::is_object() const -> bool {
            return kind() == RuntimeTypeKind::Object;
        }

        auto RuntimeType::members(const void*) const -> std::vector<RuntimeMember> {
            return {};
        }

        auto RuntimeType::member(const void*, std::string_view) const -> std::optional<RuntimeMember> {
            return std::nullopt;
        }

        Member::Member(const Member& rhs) : offset_(rhs.offset_), name(rhs.name), type_info(rhs.type_info), accessor(rhs.accessor) {
        }

        Member::Member(std::size_t offset, std::string name, std::shared_ptr<RuntimeType> type_info, Accessor accessor) :
            offset_(offset), name(std::move(name)), type_info(std::move(type_info)), accessor(std::move(accessor)) {
        }

        auto Member::data(const void* owner) const -> const void* {
            return accessor(owner);
        }

        auto Member::into_runtime(const void* owner) const -> RuntimeMember {
            return RuntimeMember{*this, owner};
        }

        RuntimeMember::RuntimeMember(const Member& meta, const void* owner) : meta(meta), owner_(owner) {
        }

        auto RuntimeMember::data() const -> const void* {
            return this->meta.data(owner_);
        }

        auto RuntimeMember::has_members() const -> bool {
            return this->meta.type_info->is_object();
        }

        auto RuntimeMember::members() const -> std::vector<RuntimeMember> {
            return this->meta.type_info->members(data());
        }

        auto RuntimeMember::get(std::string_view name) const -> std::optional<RuntimeMember> {
            return this->meta.type_info->member(data(), name);
        }

        auto RuntimeMember::to_string() const -> std::string {
            return this->meta.type_info->to_string(data());
        }
        auto RuntimeMember::to_human_string() const -> std::string {
            return this->meta.type_info->to_human_string(data());
        }
    } // namespace reflection
} // namespace ENGINE_NS
