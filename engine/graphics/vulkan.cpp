#include "engine/graphics/vulkan.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable

auto ENGINE_NS::command_pool_create_info(std::uint32_t family_index, VkCommandPoolCreateFlags flags) -> VkCommandPoolCreateInfo {
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags            = flags;
    pool_info.pNext            = nullptr;
    pool_info.queueFamilyIndex = family_index;
    return pool_info;
}

auto ENGINE_NS::command_buffer_allocate_info(VkCommandPool command_pool) -> VkCommandBufferAllocateInfo {
    VkCommandBufferAllocateInfo buffer_alloc{};
    buffer_alloc.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_alloc.pNext              = nullptr;
    buffer_alloc.commandPool        = command_pool;
    buffer_alloc.commandBufferCount = 1;
    buffer_alloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    return buffer_alloc;
}
