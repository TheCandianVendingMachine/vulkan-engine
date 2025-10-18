#include <iostream>
#include <engine/engine.h>
#include <engine/reflection/type.h>
#include <engine/ecs/component.h>

struct Example {
    int a;
    int b;
    float c;
    double d;
    unsigned int e;

    REFLECT_START(Example)
        REFLECT_MEMBER(a),
        REFLECT_MEMBER(b),
        REFLECT_MEMBER(c),
        REFLECT_MEMBER(d),
        REFLECT_MEMBER(e)
    REFLECT_END;
};

struct TestComponent: engine::ecs::Component {
    int value = 0;
    REFLECT_START(TestComponent)
        REFLECT_MEMBER(value),
    REFLECT_END;
};

int main() {
    auto component_register = engine::ecs::ComponentRegister{};
    component_register.register_component<TestComponent>();

    auto store = engine::ecs::ComponentStore<TestComponent>{};
    store.create(EntityUid(0));
    store.create(EntityUid(5));
    store.create(EntityUid(6));

    static_cast<TestComponent*>(store.fetch_mut(EntityUid(0)))->value = 5;
    static_cast<TestComponent*>(store.fetch_mut(EntityUid(5)))->value = -50;
    static_cast<TestComponent*>(store.fetch_mut(EntityUid(6)))->value = 15;

    for (auto& component : store.fetch_mut({ EntityUid(0), EntityUid(5), EntityUid(6) })) {
        auto meta = TestComponent::Meta(*static_cast<TestComponent*>(component));
        std::cout << "Component: " << meta.name << "\n";
        for (auto member : meta.members()) {
            std::cout << "\t" << member.meta.name << ": " << member.meta.type_info->name() << " = " << member.to_string() << "\n";
        }
    }

    return 0;
}
