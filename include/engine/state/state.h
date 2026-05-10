#pragma once
#include "engine/graphics/graphics.h"
#include "engine/meta_defines.h"
#include "engine/rwlock.h"

#include <memory>
#include <vector>

namespace ENGINE_NS {
    class State {
        public:
            virtual auto pre_update() -> void {
            }
            virtual auto update() -> void {
            }
            virtual auto post_update() -> void {
            }
            virtual auto update_fixed(float delta_time) -> void {
            }
            virtual auto draw(GraphicsEngine& engine, RwDataMut<graphics::FrameData>& frame) -> void {
            }

            virtual auto init_pipelines() -> std::vector<std::unique_ptr<graphics::RegisteredPipeline>> {
            }
    };
} // namespace ENGINE_NS
