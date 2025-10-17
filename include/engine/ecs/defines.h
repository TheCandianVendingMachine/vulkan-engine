#pragma once
#include "engine/meta_defines.h"
#include "engine/newtype.h"

struct EntityUid: ENGINE_NS::NewType<EntityUid, size_t>, ENGINE_NS::Eq<EntityUid>, ENGINE_NS::Hashable<EntityUid> {
    using NewType::NewType;
};

struct ComponentGid: ENGINE_NS::NewType<ComponentGid, size_t>, ENGINE_NS::Eq<ComponentGid>, ENGINE_NS::Hashable<ComponentGid> {
    using NewType::NewType;
    auto as_index() -> size_t {
        return static_cast<size_t>(*this);
    }
};

struct ComponentId: ENGINE_NS::NewType<ComponentId, size_t>, ENGINE_NS::Eq<ComponentId>, ENGINE_NS::Hashable<ComponentId>, ENGINE_NS::Increment<ComponentId> {
    using NewType::NewType;
};

template<>
struct std::hash<EntityUid>: ENGINE_NS::Hashable<EntityUid> {
};

template<>
struct std::hash<ComponentId>: ENGINE_NS::Hashable<ComponentId> {
};

template<>
struct std::hash<ComponentGid>: ENGINE_NS::Hashable<ComponentGid> {
};
