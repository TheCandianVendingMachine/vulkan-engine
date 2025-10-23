#include <engine/ecs/component.h>
#include <engine/ecs/default.h>
#include <engine/ecs/defines.h>
#include <engine/ecs/entity.h>
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

        const engine::ecs::ComponentRegister& component_register = register_;

    private:
        engine::ecs::EntityStore entities_;
        engine::ecs::ComponentRegister register_;
        tsl::robin_map<engine::ecs::ComponentGid, std::unique_ptr<engine::ecs::ComponentStoreInterface>> stores_{};
};

int main() {
    auto world = EcsWorld{};
    world.register_component<TestComponent>();
    world.register_component<engine::ecs::predefined::UidComponent>();

    auto query = world.component_register.query().select<TestComponent>().select("UidComponent").build();
    auto ent1  = world.create_entity(query);

    return 0;
}
