#pragma once
#include "engine/engine_utils.h"
#include "engine/meta_defines.h"
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>

#define VK_CHECK(x)                                                                                                                        \
    do {                                                                                                                                   \
        VkResult err = x;                                                                                                                  \
        if (err < 0) {                                                                                                                     \
            ::ENGINE_NS::vulkan_crash(err, __LINE__, __func__, __FILE__);                                                                  \
        }                                                                                                                                  \
    } while (0)
