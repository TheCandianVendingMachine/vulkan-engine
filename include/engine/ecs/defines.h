#pragma once
#include "engine/meta_defines.h"
#include "engine/newtype.h"

struct EntityUid : ENGINE_NS::NewType<EntityUid, std::size_t>, ENGINE_NS::Eq<EntityUid>, ENGINE_NS::Hashable<EntityUid> {
        using NewType::NewType;
};

struct ComponentGid : ENGINE_NS::NewType<ComponentGid, std::size_t>, ENGINE_NS::Eq<ComponentGid>, ENGINE_NS::Hashable<ComponentGid> {
        using NewType::NewType;
        auto as_index() -> std::size_t {
            return static_cast<std::size_t>(*this);
        }
};

struct ComponentId :
    ENGINE_NS::NewType<ComponentId, std::size_t>,
    ENGINE_NS::Eq<ComponentId>,
    ENGINE_NS::Hashable<ComponentId>,
    ENGINE_NS::Increment<ComponentId> {
        using NewType::NewType;
};

namespace std {
template <class Key>
struct hash;

template <>
struct hash<EntityUid> : ENGINE_NS::Hashable<EntityUid> {};

template <>
struct hash<ComponentId> : ENGINE_NS::Hashable<ComponentId> {};

template <>
struct hash<ComponentGid> : ENGINE_NS::Hashable<ComponentGid> {};
} // namespace std
