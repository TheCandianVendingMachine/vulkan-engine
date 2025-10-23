#include "engine/ecs/entity.h"

auto ENGINE_NS::ecs::EntityStore::create(const Query& query) -> EntityAllocation {
    auto borrow = m_entities.allocate(++m_current_entity);
    auto entity = *borrow.get().value();

    m_uid_borrows.insert({entity, borrow});

    auto map = Map{query.query};
    if (m_entities_with_components.contains(map)) {
        m_entities_with_components.at(map).push_back(entity);
    } else {
        m_entities_with_components.insert({map, std::vector<EntityUid>{entity}});
    }

    return EntityAllocation(entity, m_entities_with_components.find(map).key());
}

auto ENGINE_NS::ecs::EntityStore::destroy(EntityUid entity) {
    m_entities.free(m_uid_borrows.at(entity));
}

auto ENGINE_NS::ecs::EntityStore::entities_by_query(const Query& query) const -> std::vector<EntityUid> {
    std::vector<EntityUid> matching_entities{};
    for (auto& [map, entities] : m_entities_with_components) {
        if (map.assigned_components.is_subset_of(query.query)) {
            matching_entities.insert(matching_entities.end(), entities.begin(), entities.end());
        }
    }
    return matching_entities;
}

ENGINE_NS::ecs::EntityAllocation::EntityAllocation(EntityUid entity, const Map& map) : entity(entity), map(map) {
}
