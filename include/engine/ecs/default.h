#pragma once
#include "engine/ecs/component.h"
#include "engine/ecs/defines.h"
#include "engine/meta_defines.h"
#include "engine/reflection/type.h"


namespace ENGINE_NS {
    namespace ecs {
        /// Built-in component types supplied by the engine ECS.
        namespace predefined {
            /// Component that assigns a unique ComponentId when default-constructed.
            ///
            /// Add this component to an entity query when game code needs a stable
            /// per-entity id component in addition to the EntityUid handle.
            struct UidComponent : Component {
                    ComponentId id_;
                    UidComponent() {
                        static ComponentId next_id_;
                        id_ = ++next_id_;
                    }
                    REFLECT_START(UidComponent)
                    REFLECT_MEMBER(id_)
                    REFLECT_END;
            };
        } // namespace predefined
    } // namespace ecs
} // namespace ENGINE_NS
