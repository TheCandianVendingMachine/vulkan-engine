#pragma once
#include "engine/meta_defines.h"
#define VK_NO_PROTOTYPES
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace ENGINE_NS {
    struct ImageAllocation {
            VkImage image            = VK_NULL_HANDLE;
            VkImageView view         = VK_NULL_HANDLE;
            VmaAllocation allocation = VK_NULL_HANDLE;
            VkExtent3D extent{};
            VkFormat format{};
    };

} // namespace ENGINE_NS
