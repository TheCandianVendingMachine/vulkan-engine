#pragma once
#include "engine/bitset.h"
#include "engine/ecs/defines.h"
#include "engine/meta_defines.h"
#include "engine/pool.h"
#include <robin_map.h>

namespace ENGINE_NS {
namespace ecs {
struct Entity {
        EntityUid id;
};

struct Map {
        Bitset assigned_components;
};

class EntityStore {
    public:
    private:
        Pool<Entity> m_entities;
        tsl::robin_map<EntityUid, Map> m_components;
};
} // namespace ecs
} // namespace ENGINE_NS
