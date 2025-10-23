#pragma once
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
        virtual auto tick() -> void {
        }
        virtual auto fixed_tick(double dt) -> void {
        }
};
} // namespace ecs
} // namespace ENGINE_NS
