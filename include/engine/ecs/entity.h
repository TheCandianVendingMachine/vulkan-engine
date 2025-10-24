#pragma once
#include "engine/bitset.h"
#include "engine/ecs/component.h"
#include "engine/ecs/defines.h"
#include "engine/ecs/query.h"
#include "engine/meta_defines.h"
#include "engine/pool.h"
#include <robin_map.h>

namespace ENGINE_NS {
    namespace ecs {
        struct Map {
                Bitset assigned_components;

                friend auto operator==(const Map& lhs, const Map& rhs) -> bool {
                    return lhs.assigned_components == rhs.assigned_components;
                }
        };
    } // namespace ecs
} // namespace ENGINE_NS
namespace std {
    template <class Key>
    struct hash;
    template <>
    struct hash<ENGINE_NS::ecs::Map> {
            auto operator()(const ENGINE_NS::ecs::Map& map) const noexcept -> size_t {
                return map.assigned_components.hash_;
            }
    };
} // namespace std

namespace ENGINE_NS {
    namespace ecs {
        struct EntityAllocation {
                EntityUid entity;
                const Map& map;

            private:
                friend class EntityStore;
                EntityAllocation(EntityUid entity, const Map& map);
        };

        class EntityStore {
            public:
                auto create(const Query& query) -> EntityAllocation;
                auto destroy(EntityUid entity);
                auto entities_by_query(const Query& query) const -> std::vector<EntityUid>;

                auto bundles_from_query(Query query) const -> std::vector<Bundle>;

            private:
                EntityUid m_current_entity{};

                Pool<EntityUid> m_entities{};
                tsl::robin_map<EntityUid, pool::Borrow<EntityUid>> m_uid_borrows{};
                tsl::robin_map<Map, std::vector<EntityUid>> m_entities_with_components{};
        };
    } // namespace ecs
} // namespace ENGINE_NS

