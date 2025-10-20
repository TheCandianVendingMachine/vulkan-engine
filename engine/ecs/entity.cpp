#include "engine/ecs/entity.h"

auto ENGINE_NS::ecs::EntityStore::entities_by_query(const Query& query) const -> std::vector<EntityUid> {
    std::vector<EntityUid> matching_entities{};
    for (auto& [map, entities] : m_entities_with_components) {
        if (map.assigned_components.is_subset_of(query.query)) {
            matching_entities.insert(matching_entities.end(), entities.begin(), entities.end());
        }
    }
    return matching_entities;
}
