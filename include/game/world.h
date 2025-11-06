#pragma once
#include <engine/ecs/component.h>
#include <engine/ecs/defines.h>
#include <engine/ecs/entity.h>
#include <robin_map.h>

class EcsWorld {
    public:
        template <typename T>
        auto register_component() -> void {
            auto gid = register_.register_component<T>();
            stores_.insert({gid, std::make_unique<engine::ecs::ComponentStore<T>>(register_)});
        }

        auto create_entity(const engine::ecs::Query& query) -> engine::ecs::EntityUid;
        auto bundles_from_query(engine::ecs::Query& query) -> std::vector<engine::ecs::Bundle>;

        const engine::ecs::ComponentRegister& component_register = register_;

    private:
        engine::ecs::EntityStore entities_;
        engine::ecs::ComponentRegister register_;
        tsl::robin_map<engine::ecs::ComponentGid, std::unique_ptr<engine::ecs::ComponentStoreInterface>> stores_{};
};

