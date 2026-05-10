#include "engine/state/manager.h"

#include "engine/graphics/graphics.h"
#include "engine/state/state.h"

#include <type_traits>

auto ENGINE_NS::StateManager::pop() -> void {
    this->queued_pops_ += 1;
}

auto ENGINE_NS::StateManager::tick(float delta_time, GraphicsEngine& engine) -> void {
    while (this->queued_pops_ > 0) {
        this->state_stack_.back()->stop();
        this->state_stack_.back()->teardown();
        this->state_stack_.pop_back();
        if (!this->state_stack_.empty()) {
            this->state_stack_.back()->play();
        }
        this->queued_pops_ -= 1;
    }

    if (this->queued_states_.size() > 0) {
        for (auto& queued_state : this->queued_states_) {
            std::vector<std::unique_ptr<graphics::RegisteredPipeline>> converted_pipelines{};
            if (!this->state_stack_.empty()) {
                this->state_stack_.back()->stop();
            }
            this->state_stack_.emplace_back(std::move(queued_state));

            auto& back_state = this->state_stack_.back();
            back_state->setup();
            auto pipelines = back_state->init_pipelines();
            for (auto& pipeline : pipelines) {
                converted_pipelines.emplace_back(std::move(pipeline));
            }
            auto receipt                  = engine.register_pipelines(std::move(converted_pipelines));
            back_state->pipeline_receipt_ = std::move(receipt);
            back_state->play();
        }
        this->queued_states_.clear();
    }

    if (this->state_stack_.size() == 0) {
        return;
    }

    auto& state = *this->state_stack_.back();
    state.state_update_mutex_.lock();

    this->pre_update_(state);
    this->update_(state);
    this->post_update_(state);
    this->update_fixed_(state, delta_time);

    state.state_update_mutex_.unlock();
}

auto ENGINE_NS::StateManager::pre_update_(State& state) -> void {
    state.pre_update();
}

auto ENGINE_NS::StateManager::update_(State& state) -> void {
    state.update();
}

auto ENGINE_NS::StateManager::post_update_(State& state) -> void {
    state.post_update();
}

auto ENGINE_NS::StateManager::update_fixed_(State& state, float delta_time) -> void {
    state.update_fixed(delta_time);
}
