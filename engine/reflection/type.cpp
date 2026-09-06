#include "engine/reflection/type.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace ENGINE_NS {
    namespace reflection {
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

        auto RuntimeMember::to_string() const -> std::string {
            return this->meta.type_info->to_string(this->meta.data(owner_));
        }
        auto RuntimeMember::to_human_string() const -> std::string {
            return this->meta.type_info->to_human_string(this->meta.data(owner_));
        }
    } // namespace reflection
} // namespace ENGINE_NS
