#pragma once
#include "engine/graphics/scene/draw_context.h"
#include "engine/meta_defines.h"

#include <cstdint>
#include <linalg/matrix.h>
#include <vulkan/vulkan_core.h>

namespace ENGINE_NS {
    namespace scene {
        struct IRenderable {
                virtual auto draw(const ::linalg::Matrix4<float>& mvp) -> void = 0;
        };
    } // namespace scene
} // namespace ENGINE_NS
