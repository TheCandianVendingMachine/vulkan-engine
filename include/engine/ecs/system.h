#pragma once
#include "engine/ecs/component.h"
#include "engine/ecs/query.h"
#include "engine/meta_defines.h"

namespace ENGINE_NS {
    namespace ecs {
        /// Base interface for ECS systems.
        ///
        /// A System declares the component set it needs with query(), then receives
        /// matching Bundle values each tick. The engine does not own a concrete system
        /// scheduler here; game code should use EcsWorld to fetch bundles and call
        /// tick/fixed_tick from its state or world update code.
        class System {
            public:
                /// Declare the components this system reads/writes.
                ///
                /// Implementations usually return:
                ///
                ///     return component_register.query()
                ///         .select<Position>()
                ///         .select<Velocity>()
                ///         .build();
                virtual auto query(const ComponentRegister& component_register) const -> Query = 0;

                /// Called when the system is added to a scheduler/world.
                virtual auto initialise() -> void {
                }

                /// Called before the system is removed or the owning world shuts down.
                virtual auto deinitialise() -> void {
                }

                /// Per-frame update for all entities matching query().
                virtual auto tick(std::vector<Bundle>&) -> void {
                }

                /// Fixed-timestep update for all entities matching query().
                virtual auto fixed_tick(double, std::vector<Bundle>&) -> void {
                }
        };
    } // namespace ecs
} // namespace ENGINE_NS
