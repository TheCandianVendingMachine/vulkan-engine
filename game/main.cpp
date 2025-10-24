#include <engine/ecs/component.h>
#include <engine/ecs/default.h>
#include <engine/ecs/defines.h>
#include <engine/ecs/entity.h>
#include <engine/ecs/system.h>
#include <engine/engine.h>
#include <engine/reflection/type.h>
#include <iostream>
#include <robin_map.h>

struct TestComponent : engine::ecs::Component {
        int value = 0;
        REFLECT_START(TestComponent)
        REFLECT_MEMBER(value), REFLECT_END;
};

class EcsWorld {
    public:
        template <typename T>
        auto register_component() -> void {
            auto gid = register_.register_component<T>();
            stores_.insert({gid, std::make_unique<engine::ecs::ComponentStore<T>>(register_)});
        }

        auto create_entity(const engine::ecs::Query& query) -> engine::ecs::EntityUid {
            auto allocation = entities_.create(query);
            for (auto idx : allocation.map.assigned_components.set_bits()) {
                auto gid = engine::ecs::ComponentGid(idx);
                stores_.at(gid)->create(allocation.entity);
            }
            return allocation.entity;
        }

        auto bundles_from_query(engine::ecs::Query& query) -> std::vector<engine::ecs::Bundle> {
            auto bundles = entities_.bundles_from_query(query);
            for (auto& [gid, store] : stores_) {
                if (query.query.get(static_cast<std::size_t>(gid)) == 0) {
                    continue;
                }
                store->assign_bundles(bundles);
            }
            return bundles;
        }

        const engine::ecs::ComponentRegister& component_register = register_;

    private:
        engine::ecs::EntityStore entities_;
        engine::ecs::ComponentRegister register_;
        tsl::robin_map<engine::ecs::ComponentGid, std::unique_ptr<engine::ecs::ComponentStoreInterface>> stores_{};
};

class TestSystem : engine::ecs::System {
    public:
        virtual auto query(const engine::ecs::ComponentRegister& component_register) const -> engine::ecs::Query override final {
            return component_register.query().select<engine::ecs::predefined::UidComponent>().select<TestComponent>().build();
        }

        virtual auto initialise() -> void {
            fmt::println("Init");
        }
        virtual auto deinitialise() -> void {
            fmt::println("Deinit");
        }
        virtual auto tick(std::vector<engine::ecs::Bundle>& bundles) -> void {
            fmt::println("Tick");
            for (auto& bundle : bundles) {
                auto uid_component  = bundle.component<engine::ecs::predefined::UidComponent>();
                auto test_component = bundle.component<TestComponent>();
                fmt::println("Entity {}, Id {}, Test {}", static_cast<std::size_t>(bundle.entity),
                             static_cast<std::size_t>(uid_component.id_), test_component.value);
            }
        }
        virtual auto fixed_tick(double dt, std::vector<engine::ecs::Bundle>&) -> void {
            fmt::println("Fixed Tick ({:.2f})", dt);
        }
};

int main() {
    auto engine = engine::Engine();

    auto world  = EcsWorld{};
    world.register_component<TestComponent>();
    world.register_component<engine::ecs::predefined::UidComponent>();

    auto ent1 = world.create_entity(world.component_register.query().select<TestComponent>().select("UidComponent").build());

    TestSystem system{};
    system.initialise();

    for (int i = 0; i < 500; i++) {
        auto query   = system.query(world.component_register);
        auto bundles = world.bundles_from_query(query);
        system.tick(bundles);
    }

    system.deinitialise();

    return 0;
}
