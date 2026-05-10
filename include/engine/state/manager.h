#pragma once
#include "engine/graphics/graphics.h"
#include "engine/meta_defines.h"
#include "engine/state/state.h"

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
            queued_states_.push_back(T(std::forward<Args>(args...));
            }

            auto tick(float delta_time, GraphicsEngine& engine) -> void;

        private:
            auto pre_update_() -> void;
            auto update_() -> void;
            auto post_update_() -> void;
            auto update_fixed_(float delta_time) -> void;
            auto draw_(GraphicsEngine& engine) -> void;

        private:
            std::vector<std::unique_ptr<State>> queued_states_ = {};
            std::vector<std::unique_ptr<State>> state_stack_   = {};
    };
} // namespace ENGINE_NS
