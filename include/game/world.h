#pragma once
#include <engine/ecs/component.h>
#include <engine/ecs/defines.h>
#include <engine/ecs/entity.h>
#include <memory>
#include <vector>
#include <robin_map.h>

/// Game-facing ECS world that combines engine ECS primitives.
///
/// The engine ECS separates entity ids, component type registration, component
/// storage, and queries. EcsWorld is the game-layer convenience owner that keeps
/// those pieces together:
///
/// - register components with register_component<T>() during setup;
/// - build entity/query signatures with component_register.query();
/// - create entities with create_entity(query);
/// - fetch mutable bundles for systems with bundles_from_query(query).
class EcsWorld {
    public:
        /// Register component type T with the world and allocate its store.
        ///
        /// T must derive from engine::ecs::Component and expose reflection metadata
        /// through REFLECT_START/REFLECT_MEMBER/REFLECT_END. Register every component
        /// type before creating entities or building systems that select it.
        template <typename T>
        auto register_component() -> void {
            auto gid = register_.register_component<T>();
            stores_.insert({gid, std::make_unique<engine::ecs::ComponentStore<T>>(register_)});
        }

        /// Create an entity and default-construct the components selected by query.
        ///
        /// The query acts as the entity's full component signature. For each selected
        /// component type, EcsWorld looks up the matching ComponentStore and creates a
        /// default component instance assigned to the new EntityUid.
        auto create_entity(const engine::ecs::Query& query) -> engine::ecs::EntityUid;

        /// Fetch mutable component bundles for every entity matching query.
        ///
        /// Use this from systems or game update code. Each returned Bundle contains
        /// the entity id plus pointers to the components selected by query.
        auto bundles_from_query(engine::ecs::Query& query) -> std::vector<engine::ecs::Bundle>;

        /// Component registry used to build queries for this world.
        const engine::ecs::ComponentRegister& component_register = register_;

    private:
        engine::ecs::EntityStore entities_;
        engine::ecs::ComponentRegister register_;
        tsl::robin_map<engine::ecs::ComponentGid, std::unique_ptr<engine::ecs::ComponentStoreInterface>> stores_{};
};

