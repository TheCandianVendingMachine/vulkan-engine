#include "engine/state/state.h"

#include "engine/meta_defines.h"

#include <memory>
#include <vector>

ENGINE_NS::StatePipeline::StatePipeline(State& state) : owner_(state) {
}

ENGINE_NS::StatePipeline::~StatePipeline() {
}

auto ENGINE_NS::StatePipeline::record(VkCommandBuffer buffer) -> void {
    this->owner_.state_update_mutex_.lock();
    this->record_(buffer);
    this->owner_.state_update_mutex_.unlock();
}

auto ENGINE_NS::State::setup() -> void {
}

auto ENGINE_NS::State::teardown() -> void {
}

auto ENGINE_NS::State::play() -> void {
}

auto ENGINE_NS::State::stop() -> void {
}

auto ENGINE_NS::State::pre_update() -> void {
}

auto ENGINE_NS::State::update() -> void {
}

auto ENGINE_NS::State::post_update() -> void {
}

auto ENGINE_NS::State::update_fixed(double) -> void {
}

auto ENGINE_NS::State::init_pipelines(engine::GraphicsEngine&) -> std::vector<std::unique_ptr<StatePipeline>> {
    return {};
}
