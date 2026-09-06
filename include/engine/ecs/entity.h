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
        /// Component signature for an entity archetype.
        ///
        /// Map stores the exact component set assigned to an entity. EntityStore uses
        /// it as an archetype key so queries can scan maps and return matching entity
        /// ids.
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
        /// Result of allocating an entity in EntityStore.
        ///
        /// EcsWorld uses this to create matching component instances in each
        /// ComponentStore. `map` is a stable reference to the stored archetype key.
        struct EntityAllocation {
                EntityUid entity;
                const Map& map;

            private:
                friend class EntityStore;
                EntityAllocation(EntityUid entity, const Map& map);
        };

        /// Owns entity ids and maps entities to their component signatures.
        ///
        /// EntityStore does not store component data. It only tracks which entities
        /// exist and which component type set each entity has. EcsWorld combines this
        /// with ComponentStore<T> instances to provide complete ECS behavior.
        class EntityStore {
            public:
                /// Create an entity whose component signature exactly matches query.
                auto create(const Query& query) -> EntityAllocation;

                /// Destroy an entity id.
                ///
                /// Component destruction is the responsibility of the world/store layer
                /// that owns ComponentStoreInterface instances.
                auto destroy(EntityUid entity);

                /// Return entities whose component signatures include all query bits.
                auto entities_by_query(const Query& query) const -> std::vector<EntityUid>;

                /// Create bundles for entities matching query.
                ///
                /// The returned bundles do not contain component pointers yet. EcsWorld
                /// fills them by asking each relevant ComponentStore to assign itself.
                auto bundles_from_query(Query query) const -> std::vector<Bundle>;

            private:
                EntityUid m_current_entity{};

                Pool<EntityUid> m_entities{};
                tsl::robin_map<EntityUid, pool::Borrow<EntityUid>> m_uid_borrows{};
                tsl::robin_map<Map, std::vector<EntityUid>> m_entities_with_components{};
        };
    } // namespace ecs
} // namespace ENGINE_NS

