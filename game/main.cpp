#include "game/tilemap/tilemap.h"

#include <engine/assets/library.h>
#include <engine/deletion_queue.h>
#include <engine/engine.h>
#include <engine/engine_utils.h>
#include <engine/graphics/pipeline.h>
#include <engine/meta_defines.h>
#include <engine/state/state.h>
#include <memory>
#include <utility>
#include <vector>

class TestState : public engine::State {
    public:
        TileMap tilemap;

        TestState() : tilemap{16, 16, 64, Tile{}} {
        }

        virtual auto update() -> void override final {
            engine::g_ENGINE->logger.get(engine::LogNamespaces::GAME).get().info("hiii");
        }

        virtual auto init_pipelines(engine::GraphicsEngine&) -> std::vector<std::unique_ptr<engine::StatePipeline>> override final {
            auto pipelines = std::vector<std::unique_ptr<engine::StatePipeline>>{};
            pipelines.emplace_back(std::make_unique<TilemapPipeline>(*this));
            return pipelines;
        }
};

int main() {
    engine::Engine engine{};
    engine.state_manager.queue<TestState>();
    engine.run();
    return 0;
}
