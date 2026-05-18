#pragma once
#include "engine/engine_utils.h"
#include "engine/meta_defines.h"

#include <SDL3/SDL_events.h>
#include <imgui.h>
#include <vulkan/vulkan_core.h>

#include <client/TracyProfiler.hpp>
#include <vector>

#define VK_CHECK(x)                                                                                                                        \
    do {                                                                                                                                   \
        VkResult err = x;                                                                                                                  \
        if (err < 0) {                                                                                                                     \
            ::ENGINE_NS::vulkan_crash(err, __LINE__, __func__, __FILE__);                                                                  \
        }                                                                                                                                  \
    } while (0)

namespace tracy {
    class VkCtx;
}
namespace ENGINE_NS {
    namespace graphics {
        struct ImGui {
                VkQueue queue                    = VK_NULL_HANDLE;
                VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
                ImDrawData* latest_draw          = nullptr;
                std::vector<SDL_Event> events{};

                int frame_ = -1;
                void start_frame();
        };

        struct Immediate {
                VkFence fence                  = VK_NULL_HANDLE;
                VkCommandBuffer command_buffer = VK_NULL_HANDLE;
                VkCommandPool command_pool     = VK_NULL_HANDLE;
                VkQueue queue                  = VK_NULL_HANDLE;
                tracy::VkCtx* tracy_context    = nullptr;

                auto setup(VkDevice device) const -> void;
                auto teardown(VkDevice device) const -> void;
        };
    } // namespace graphics
} // namespace ENGINE_NS
