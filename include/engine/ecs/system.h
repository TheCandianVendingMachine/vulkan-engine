#pragma once
#include "engine/ecs/component.h"
#include "engine/ecs/query.h"
#include "engine/meta_defines.h"

namespace ENGINE_NS {
    namespace ecs {
        class System {
            public:
                virtual auto query() -> Query = 0;

                virtual auto initialise() -> void {
                }
                virtual auto deinitialise() -> void {
                }
                virtual auto tick(std::vector<Bundle>& bundles) -> void {
                }
                virtual auto fixed_tick(double dt, std::vector<Bundle>& bundles) -> void {
                }
        };
    } // namespace ecs
} // namespace ENGINE_NS
