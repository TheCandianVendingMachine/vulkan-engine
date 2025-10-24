#include "engine/ecs/component.h"
#include "engine/ecs/default.h"
#include "engine/ecs/defines.h"
#include "engine/meta_defines.h"
#include <Tracy/tracy.hpp>
#include <utility>

ENGINE_NS::ecs::ComponentRegister::ComponentRegister() {
}

auto ENGINE_NS::ecs::ComponentRegister::register_component_by_name(std::string_view name) -> ComponentGid {
    ZoneScoped;
    auto current_gid = counter_;
    register_.insert({std::string(name), current_gid});
    counter_ = ComponentGid(static_cast<underlying_type<ComponentGid>>(current_gid) + 1);
    return current_gid;
}

auto ENGINE_NS::ecs::ComponentRegister::component_gid_by_name(std::string_view name) const -> std::optional<ComponentGid> {
    ZoneScoped;
    if (!register_.contains(std::string(name))) {
        return std::nullopt;
    }
    return std::optional<ComponentGid>{register_.at(std::string(name))};
}

auto ENGINE_NS::ecs::ComponentRegister::query() const -> QueryBuilder {
    return QueryBuilder(*this);
}

auto ENGINE_NS::ecs::ComponentStoreInterface::fetch_mut(const std::vector<EntityUid>& entities) -> std::vector<Component*> {
    ZoneScoped;
    auto components = fetch(entities);
    std::vector<Component*> mutable_components{};
    mutable_components.reserve(components.size());
    std::transform(components.begin(), components.end(), std::back_inserter(mutable_components), [](const Component* c) {
        return const_cast<Component*>(c);
    });
    return mutable_components;
}

auto ENGINE_NS::ecs::ComponentStoreInterface::fetch_mut(EntityUid entity) -> Component* {
    ZoneScoped;
    return const_cast<Component*>(fetch(entity));
}

ENGINE_NS::ecs::Bundle::Bundle(Bundle&& rhs) : entity_(std::move(rhs.entity_)), query_(std::move(rhs.query_)) {
}

ENGINE_NS::ecs::Bundle::Bundle(EntityUid entity, Query query) : entity_(entity), query_(std::move(query)) {
}
