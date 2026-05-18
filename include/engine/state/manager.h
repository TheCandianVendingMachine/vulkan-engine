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
                queued_states_.emplace_back(std::make_unique<T>());
            }
            template <typename T, typename... Args>
            auto queue(Args&&... args) -> void {
                queued_states_.emplace_back(std::make_unique<T>(std::forward<Args...>(args...)));
            }

            auto pop() -> void;

            auto start_frame(GraphicsEngine& engine) -> void;
            auto pre_update() -> void;
            auto update() -> void;
            auto post_update() -> void;
            auto update_fixed(double delta_time) -> void;
            auto end_frame() -> void;

            auto shutdown(GraphicsEngine& engine) -> void;

        private:
            std::vector<std::unique_ptr<State>> queued_states_;
            std::vector<std::unique_ptr<State>> state_stack_;
            std::uint64_t queued_pops_ = 0;
    };
} // namespace ENGINE_NS
