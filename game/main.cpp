#include "engine/ecs/component.h"
#include "engine/reflection/type.h"
#include "engine/utilities/transform.h"
#include "game/input/types.h"
#include "game/tilemap/tilemap.h"
#include "game/world.h"
#include "linalg/vector.h"

#include <SDL3/SDL_events.h>
#include <engine/assets/library.h>
#include <engine/deletion_queue.h>
#include <engine/ecs/query.h>
#include <engine/ecs/system.h>
#include <engine/engine.h>
#include <engine/engine_utils.h>
#include <engine/graphics/pipeline.h>
#include <engine/linalg/vector_operations.h>
#include <engine/meta_defines.h>
#include <engine/state/state.h>

#include <memory>
#include <vector>


struct TransformComponent : engine::ecs::Component {
        engine::Transform transform;
        REFLECT_START(TransformComponent)
        REFLECT_MEMBER(transform)
        REFLECT_END;
};

struct PointMassComponent : engine::ecs::Component {
        linalg::Vector3<double> velocity;
        linalg::Vector3<double> acceleration;
        double mass = 0.0;
        REFLECT_START(PointMassComponent)
        REFLECT_MEMBER(velocity), REFLECT_MEMBER(acceleration), REFLECT_MEMBER(mass), REFLECT_END;
};


struct PhysicsSystem : public engine::ecs::System {
        double time = 0.0;
        auto query(const engine::ecs::ComponentRegister& component_register) const -> engine::ecs::Query final {
            return component_register.query().select<TransformComponent>().select<PointMassComponent>().build();
        }

        auto tick(std::vector<engine::ecs::Bundle>& entities) -> void final {
            if (time >= 0.3) {
                for (auto& entity : entities) {
                    auto& position = entity.component<TransformComponent>().transform.position;
                    engine::g_ENGINE->logger.get(engine::LogNamespaces::GAME)
                        .get()
                        .debug("{:.2f}, {:.2f}, {:.2f}", position.x, position.y, position.z);
                }
                time = 0.0;
            }
        }

        auto fixed_tick(double delta_time, std::vector<engine::ecs::Bundle>& entities) -> void final {
            time += delta_time;
            for (auto& entity : entities) {
                auto& transform = entity.component<TransformComponent>();
                auto& dynamics  = entity.component<PointMassComponent>();

                dynamics.acceleration = linalg::Vector3<double>{0.0, -9.81, 0.0};
                dynamics.velocity += dynamics.acceleration * delta_time;
                transform.transform.translate(dynamics.velocity * delta_time);
            }
        }
};

class TestState : public engine::State {
    public:
        TileMap tilemap;
        EcsWorld ecs;

        PhysicsSystem physics_;

        TestState() : tilemap{16, 16, 64, Tile{}} {
            ecs.register_component<TransformComponent>();
            ecs.register_component<PointMassComponent>();

            auto player_query = ecs.component_register.query().select<TransformComponent>().select<PointMassComponent>().build();

            ecs.create_entity(player_query);
        }

        auto update() -> void final {
            auto query  = physics_.query(ecs.component_register);
            auto bundle = ecs.bundles_from_query(query);
            physics_.tick(bundle);
        }

        auto update_fixed(double delta_time) -> void final {
            auto query  = physics_.query(ecs.component_register);
            auto bundle = ecs.bundles_from_query(query);
            physics_.fixed_tick(delta_time, bundle);
        }

        auto init_pipelines(engine::GraphicsEngine&) -> std::vector<std::unique_ptr<engine::StatePipeline>> final {
            auto pipelines = std::vector<std::unique_ptr<engine::StatePipeline>>{};
            pipelines.emplace_back(std::make_unique<TilemapDrawPipeline>(*this, tilemap));
            return pipelines;
        }
};

int main() {
    engine::Engine engine{};
    engine.state_manager.queue<TestState>();
    engine.run();
    return 0;
}
