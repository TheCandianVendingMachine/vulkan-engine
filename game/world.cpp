#include "game/world.h"
#include <tracy/Tracy.hpp>

auto EcsWorld::create_entity(const engine::ecs::Query& query) -> engine::ecs::EntityUid {
    ZoneScoped;
    auto allocation = entities_.create(query);
    for (auto idx : allocation.map.assigned_components.set_bits()) {
        auto gid = engine::ecs::ComponentGid(idx);
        stores_.at(gid)->create(allocation.entity);
    }
    return allocation.entity;
}

auto EcsWorld::bundles_from_query(engine::ecs::Query& query) -> std::vector<engine::ecs::Bundle> {
    ZoneScoped;
    auto bundles = entities_.bundles_from_query(query);
    for (auto& [gid, store] : stores_) {
        if (query.query.get(static_cast<std::size_t>(gid)) == 0) {
            continue;
        }
        store->assign_bundles(bundles);
    }
    return bundles;
}
