#pragma once
#include "engine/engine_utils.h"
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>

namespace ENGINE_NS {
    struct ComputePipeline {
            VkPipeline pipeline              = VK_NULL_HANDLE;
            VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    };
} // namespace ENGINE_NS
