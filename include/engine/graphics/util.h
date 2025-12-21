#pragma once
#include "engine/engine_utils.h"
#include "engine/meta_defines.h"
#define VK_NO_PROTOTYPES
#include <SDL3/SDL_events.h>
#include <vector>
#include <vulkan/vulkan_core.h>

#define VK_CHECK(x)                                                                                                                        \
    do {                                                                                                                                   \
        VkResult err = x;                                                                                                                  \
        if (err < 0) {                                                                                                                     \
            ::ENGINE_NS::vulkan_crash(err, __LINE__, __func__, __FILE__);                                                                  \
        }                                                                                                                                  \
    } while (0)

namespace ENGINE_NS {
    namespace graphics {
        struct ImGui {
                VkQueue queue                    = VK_NULL_HANDLE;
                VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
                std::vector<SDL_Event> events{};

                int frame_ = -1;
                void start_frame();
        };
    } // namespace graphics
} // namespace ENGINE_NS
