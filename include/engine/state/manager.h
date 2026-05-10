#pragma once
#include "engine/graphics/graphics.h"
#include "engine/meta_defines.h"
#include "engine/state/state.h"

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace ENGINE_NS {
    class StateManager {
        public:
            template <typename T>
            auto queue() -> void {
                queued_states_.push_back(T{});
            }
            template <typename T, typename... Args>
            auto queue(Args&&... args) -> void {
                queued_states_.push_back(std::make_unique<T>(std::forward<Args>(args...)));
            }

            auto pop() -> void;

            auto tick(float delta_time, GraphicsEngine& engine) -> void;

        private:
            auto pre_update_(State& state) -> void;
            auto update_(State& state) -> void;
            auto post_update_(State& state) -> void;
            auto update_fixed_(State& state, float delta_time) -> void;

        private:
            std::vector<std::unique_ptr<State>> queued_states_ = {};
            std::vector<std::unique_ptr<State>> state_stack_   = {};
            std::uint64_t queued_pops_                         = 0;
    };
} // namespace ENGINE_NS
