#include "engine/state/manager.h"

#include "engine/engine.h"
#include "engine/graphics/graphics.h"
#include "engine/logger.h"
#include "engine/state/state.h"

#include <memory>
#include <type_traits>
#include <vector>

auto ENGINE_NS::StateManager::pop() -> void {
    this->queued_pops_ += 1;
}

auto ENGINE_NS::StateManager::start_frame(GraphicsEngine& engine) -> void {
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GAMESTATE);

    while (this->queued_pops_ > 0) {
        auto& back_state = this->state_stack_.back();
        logger.get().debug("Popping state");
        back_state->stop();
        back_state->teardown();
        this->state_stack_.pop_back();
        engine.resume_registered_pipelines();
        if (!this->state_stack_.empty()) {
            this->state_stack_.back()->play();
        }
        this->queued_pops_ -= 1;
    }

    if (this->queued_states_.size() > 0) {
        for (auto& queued_state : this->queued_states_) {
            logger.get().debug("Pushing state");

            std::vector<std::unique_ptr<graphics::RegisteredPipeline>> converted_pipelines{};
            if (!this->state_stack_.empty()) {
                this->state_stack_.back()->stop();
            }
            engine.pause_registered_pipelines();

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
    this->state_stack_.back()->state_update_mutex_.lock();
}

auto ENGINE_NS::StateManager::pre_update() -> void {
    if (this->state_stack_.size() == 0) {
        return;
    }
    this->state_stack_.back()->pre_update();
}

auto ENGINE_NS::StateManager::update() -> void {
    if (this->state_stack_.size() == 0) {
        return;
    }
    this->state_stack_.back()->update();
}

auto ENGINE_NS::StateManager::post_update() -> void {
    if (this->state_stack_.size() == 0) {
        return;
    }
    this->state_stack_.back()->post_update();
}

auto ENGINE_NS::StateManager::update_fixed(double delta_time) -> void {
    if (this->state_stack_.size() == 0) {
        return;
    }
    this->state_stack_.back()->update_fixed(delta_time);
}

auto ENGINE_NS::StateManager::end_frame() -> void {
    if (this->state_stack_.size() == 0) {
        return;
    }
    this->state_stack_.back()->state_update_mutex_.unlock();
}
