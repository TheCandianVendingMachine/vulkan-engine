#include "engine/ecs/component.h"
#include "engine/ecs/defines.h"
#include "engine/meta_defines.h"

auto ENGINE_NS::ecs::ComponentRegister::register_component_by_name(std::string_view name) -> ComponentGid {
    auto current_gid = counter_;
    register_.insert({std::string(name), current_gid});
    counter_ = ComponentGid(static_cast<underlying_type<ComponentGid>>(current_gid) + 1);
    return current_gid;
}

auto ENGINE_NS::ecs::ComponentRegister::component_gid_by_name(std::string_view name) -> std::optional<ComponentGid> {
    if (!register_.contains(std::string(name))) {
        return std::nullopt;
    }
    return std::optional<ComponentGid>{register_.at(std::string(name))};
}

auto ENGINE_NS::ecs::ComponentStoreInterface::fetch_mut(const std::vector<EntityUid>& entities) -> std::vector<Component*> {
    auto components = fetch(entities);
    std::vector<Component*> mutable_components{};
    mutable_components.reserve(components.size());
    std::transform(components.begin(), components.end(), std::back_inserter(mutable_components), [](const Component* c) {
        return const_cast<Component*>(c);
    });
    return mutable_components;
}

auto ENGINE_NS::ecs::ComponentStoreInterface::fetch_mut(EntityUid entity) -> Component* {
    return const_cast<Component*>(fetch(entity));
}
