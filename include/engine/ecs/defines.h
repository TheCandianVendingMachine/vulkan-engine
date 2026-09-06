#pragma once
#include "engine/meta_defines.h"
#include "engine/newtype.h"

namespace ENGINE_NS {
    namespace ecs {
        /// Stable identifier for an entity allocated by an EntityStore.
        ///
        /// EntityUid values are intentionally lightweight handles. Component data is
        /// stored separately in ComponentStore instances and is looked up by this id.
        struct EntityUid :
            ENGINE_NS::NewType<EntityUid, std::size_t>,
            ENGINE_NS::Eq<EntityUid>,
            ENGINE_NS::Hashable<EntityUid>,
            ENGINE_NS::Increment<EntityUid> {
                using NewType::NewType;
        };

        /// Global component type id assigned by ComponentRegister.
        ///
        /// A ComponentGid identifies a component *type*, not a component instance.
        /// It is used as the bit index in Query/Map bitsets and as the key for the
        /// component store owned by EcsWorld.
        struct ComponentGid :
            ENGINE_NS::NewType<ComponentGid, std::size_t>,
            ENGINE_NS::Eq<ComponentGid>,
            ENGINE_NS::Hashable<ComponentGid> {
                using NewType::NewType;

                /// Return this component type id as a zero-based bitset/store index.
                auto as_index() -> std::size_t {
                    return static_cast<std::size_t>(*this);
                }
        };

        /// Per-component-instance id for components that need their own identity.
        ///
        /// Most components do not need this directly. The predefined UidComponent
        /// uses it to attach a unique id component to an entity.
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
