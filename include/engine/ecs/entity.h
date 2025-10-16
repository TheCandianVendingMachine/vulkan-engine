#pragma once
#include "engine/meta_defines.h"
#include "engine/ecs/defines.h"
#include "engine/pool.h"
#include "engine/bitset.h"
#include "robin_map.h"

namespace ENGINE_NS {
    namespace ecs {
        struct Entity {
            Uid id;
        };

        struct Map {
            Bitset assigned_components;
        };

        class EntityStore {
            public:
            private:
                Pool<Entity> m_entities;
                tsl::robin_map<Uid, Map> m_components;
        };
    }
}

