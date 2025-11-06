#include <engine/ecs/component.h>
#include <engine/ecs/default.h>
#include <engine/ecs/defines.h>
#include <engine/ecs/entity.h>
#include <engine/ecs/system.h>
#include <engine/engine.h>
#include <engine/linalg/matrix.h>
#include <engine/reflection/type.h>
#include <iostream>
#include <robin_map.h>

#include <tracy/Tracy.hpp>

#include "game/world.h"

struct TestComponent : engine::ecs::Component {
        int value = 0;
        REFLECT_START(TestComponent)
        REFLECT_MEMBER(value), REFLECT_END;
};

class TestSystem : engine::ecs::System {
    public:
        virtual auto query(const engine::ecs::ComponentRegister& component_register) const -> engine::ecs::Query override final {
            return component_register.query().select<engine::ecs::predefined::UidComponent>().select<TestComponent>().build();
        }

        virtual auto initialise() -> void {
        }
        virtual auto deinitialise() -> void {
        }
        virtual auto tick(std::vector<engine::ecs::Bundle>& bundles) -> void {
            for (auto& bundle : bundles) {
                auto uid_component  = bundle.component<engine::ecs::predefined::UidComponent>();
                auto test_component = bundle.component<TestComponent>();
            }
        }
        virtual auto fixed_tick(double, std::vector<engine::ecs::Bundle>&) -> void {
        }
};

int main() {
    for (volatile unsigned long long i = 0; i < 6'000'000'000; i++) {
    }
    auto engine = engine::Engine();

    auto A      = linalg::Matrix4<float>::identity();
    auto B      = linalg::Matrix4<float>::identity();

    A.c1r1      = 5.f;
    A.c2r4      = 2.f;
    A.c1r2      = 5.f;

    B.r3c2      = 2.f;
    B.r1c1      = 0.f;
    B.r1c3      = 2.f;

    auto C      = A * B;

    fmt::println("{}", C.r2c3);

    auto world = EcsWorld{};
    world.register_component<TestComponent>();
    world.register_component<engine::ecs::predefined::UidComponent>();

    auto ent1 = world.create_entity(world.component_register.query().select<TestComponent>().select("UidComponent").build());

    TestSystem system{};
    system.initialise();

    for (int i = 0; i < 50'000; i++) {
        auto query   = system.query(world.component_register);
        auto bundles = world.bundles_from_query(query);
        system.tick(bundles);
        C = C * A;
        C = B * C;
        FrameMark;
    }

    system.deinitialise();

    return 0;
}
