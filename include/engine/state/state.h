#pragma once
#include "engine/deletion_queue.h"
#include "engine/graphics/graphics.h"
#include "engine/graphics/pipeline.h"
#include "engine/meta_defines.h"

#include <Tracy/Tracy.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace ENGINE_NS {
    class State;
    class StatePipeline : public graphics::RegisteredPipeline {
        public:
            StatePipeline(State& state);
            virtual ~StatePipeline();

            virtual auto record(VkCommandBuffer buffer) -> void override final;

            virtual auto name() const -> std::string                                                                        = 0;
            virtual auto build_pipeline(engine::GraphicsEngine& engine,
                                        VulkanDevice& device,
                                        GraphicsRegisteredPipelineDeletionQueue& deletion_queue) -> GraphicsPipelineBuilder = 0;

        protected:
            virtual auto record_(VkCommandBuffer buffer) -> void = 0;

            State& owner_;
    };

    class State {
        public:
            virtual ~State() = default;

            virtual auto setup() -> void;
            virtual auto teardown() -> void;
            virtual auto play() -> void;
            virtual auto stop() -> void;
            virtual auto pre_update() -> void;
            virtual auto update() -> void;
            virtual auto post_update() -> void;
            virtual auto update_fixed(double delta_time) -> void;

            virtual auto init_pipelines(engine::GraphicsEngine& engine) -> std::vector<std::unique_ptr<StatePipeline>>;

        private:
            friend class StatePipeline;
            friend class StateManager;
            std::optional<graphics::RegisteredPipelineReceipt> pipeline_receipt_ = std::nullopt;
            TracyLockable(std::mutex, state_update_mutex_);
    };
} // namespace ENGINE_NS
