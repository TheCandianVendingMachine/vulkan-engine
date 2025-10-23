#pragma once
#include "engine/meta_defines.h"
#include "engine/newtype.h"

namespace ENGINE_NS {
    namespace ecs {
        struct EntityUid :
            ENGINE_NS::NewType<EntityUid, std::size_t>,
            ENGINE_NS::Eq<EntityUid>,
            ENGINE_NS::Hashable<EntityUid>,
            ENGINE_NS::Increment<EntityUid> {
                using NewType::NewType;
        };

        struct ComponentGid :
            ENGINE_NS::NewType<ComponentGid, std::size_t>,
            ENGINE_NS::Eq<ComponentGid>,
            ENGINE_NS::Hashable<ComponentGid> {
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

    } // namespace ecs
} // namespace ENGINE_NS


namespace std {
    template <class Key>
    struct hash;

    template <>
    struct hash<ENGINE_NS::ecs::EntityUid> : ENGINE_NS::Hashable<ENGINE_NS::ecs::EntityUid> {};

    template <>
    struct hash<ENGINE_NS::ecs::ComponentId> : ENGINE_NS::Hashable<ENGINE_NS::ecs::ComponentId> {};

    template <>
    struct hash<ENGINE_NS::ecs::ComponentGid> : ENGINE_NS::Hashable<ENGINE_NS::ecs::ComponentGid> {};
} // namespace std
