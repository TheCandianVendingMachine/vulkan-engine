# ECS guide

The ECS is split between reusable engine primitives under `engine::ecs` and the game-facing `EcsWorld` class in `include/game/world.h`.

## Core concepts

- `engine::ecs::Component` is the base class for component data.
- `engine::ecs::ComponentRegister` assigns each component type a global id.
- `engine::ecs::Query` is a component selection bitset.
- `engine::ecs::EntityStore` owns entity ids and entity component signatures.
- `engine::ecs::ComponentStore<T>` owns component instances for one component type.
- `engine::ecs::Bundle` is what systems operate on: one entity plus its selected component pointers.
- `EcsWorld` ties the pieces together for game code.

## Adding a component

Define a small data type that derives from `engine::ecs::Component` and has reflection metadata:

```cpp
#include <engine/ecs/component.h>
#include <engine/reflection/type.h>

struct Position : engine::ecs::Component {
    float x = 0.0f;
    float y = 0.0f;

    REFLECT_START(Position)
    REFLECT_MEMBER(x), REFLECT_MEMBER(y)
    REFLECT_END
};
```

Register the component with your `EcsWorld` before creating entities that use it:

```cpp
EcsWorld world;
world.register_component<Position>();
```

## Creating an entity

Build a query from the world's component register. The query is the full component signature for the entity being created:

```cpp
auto player_signature = world.component_register.query()
    .select<Position>()
    .select<Velocity>()
    .build();

engine::ecs::EntityUid player = world.create_entity(player_signature);
```

`EcsWorld::create_entity` creates the entity id and default-constructs one component instance in each selected `ComponentStore<T>`.

## Adding a system

A system derives from `engine::ecs::System`, declares the components it needs in `query`, and updates matching bundles in `tick` or `fixed_tick`:

```cpp
#include <engine/ecs/system.h>

class MovementSystem : public engine::ecs::System {
public:
    auto query(const engine::ecs::ComponentRegister& components) const -> engine::ecs::Query override {
        return components.query()
            .select<Position>()
            .select<Velocity>()
            .build();
    }

    auto fixed_tick(double dt, std::vector<engine::ecs::Bundle>& bundles) -> void override {
        for (auto& bundle : bundles) {
            auto& position = bundle.component<Position>();
            auto& velocity = bundle.component<Velocity>();

            position.x += velocity.x * static_cast<float>(dt);
            position.y += velocity.y * static_cast<float>(dt);
        }
    }
};
```

## Running systems with `EcsWorld`

There is not currently a dedicated engine-owned system scheduler. Game code should keep systems and call them from a state/world update loop:

```cpp
MovementSystem movement;

auto movement_query = movement.query(world.component_register);
auto bundles = world.bundles_from_query(movement_query);
movement.fixed_tick(delta_time, bundles);
```

The important rule is that queries must be built from the same `EcsWorld::component_register` that registered the components.

## Practical setup order

1. Construct `EcsWorld`.
2. Register all component types.
3. Construct systems or build their queries.
4. Create entities using queries/signatures.
5. During update, ask `EcsWorld` for bundles matching each system query.
6. Pass those bundles to the system.

## Notes and current limitations

- Components are default-constructed when an entity is created. Set initial values by fetching a bundle/query after creation, or add helper APIs around `EcsWorld` later.
- `Bundle::component<T>()` assumes `T` was selected by the query used to create the bundle.
- `EntityStore::destroy` currently destroys the entity id only; component destruction should be coordinated by the world layer.
- The ECS is currently simple and archetype-like: entity component signatures are fixed at creation time.
