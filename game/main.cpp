#include <chrono>
#include <engine/assets/library.h>
#include <engine/engine.h>
#include <engine/state/state.h>
#include <thread>

class TestState : public engine::State {
    public:
        auto update() -> void {
            engine::g_ENGINE->logger.get(engine::LogNamespaces::GAME).get().info("hiii");
        }
};

int main() {
    engine::Engine engine{};
    engine.state_manager.queue<TestState>();
    engine.run();
    return 0;
}
