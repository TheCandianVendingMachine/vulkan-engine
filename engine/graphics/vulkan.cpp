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

auto ENGINE_NS::command_buffer_begin_info(VkCommandBufferUsageFlags flags) -> VkCommandBufferBeginInfo {
    VkCommandBufferBeginInfo info{};
    info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext            = nullptr;
    info.pInheritanceInfo = nullptr;
    info.flags            = flags;
    return info;
}

auto ENGINE_NS::fence_create_info(VkFenceCreateFlags flags) -> VkFenceCreateInfo {
    VkFenceCreateInfo info = {};
    info.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext             = nullptr;
    info.flags             = flags;
    return info;
}

auto ENGINE_NS::semaphore_create_info(VkSemaphoreCreateFlags flags) -> VkSemaphoreCreateInfo {
    VkSemaphoreCreateInfo info = {};
    info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext                 = nullptr;
    info.flags                 = flags;

    return info;
}

auto ENGINE_NS::semaphore_submit_info(VkPipelineStageFlags2 stage_mask, VkSemaphore semaphore) -> VkSemaphoreSubmitInfo {
    VkSemaphoreSubmitInfo submit_info{};
    submit_info.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    submit_info.pNext       = nullptr;
    submit_info.semaphore   = semaphore;
    submit_info.stageMask   = stage_mask;
    submit_info.deviceIndex = 0;
    submit_info.value       = 1;

    return submit_info;
}

auto ENGINE_NS::command_buffer_submit_info(VkCommandBuffer command_buffer) -> VkCommandBufferSubmitInfo {
    VkCommandBufferSubmitInfo info{};
    info.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    info.pNext         = nullptr;
    info.commandBuffer = command_buffer;
    info.deviceMask    = 0;

    return info;
}

auto ENGINE_NS::submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signal_semaphore_info,
                            VkSemaphoreSubmitInfo* wait_semaphore_info) -> VkSubmitInfo2 {
    VkSubmitInfo2 info            = {};
    info.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    info.pNext                    = nullptr;

    info.waitSemaphoreInfoCount   = wait_semaphore_info == nullptr ? 0 : 1;
    info.pWaitSemaphoreInfos      = wait_semaphore_info;

    info.signalSemaphoreInfoCount = signal_semaphore_info == nullptr ? 0 : 1;
    info.pSignalSemaphoreInfos    = signal_semaphore_info;

    info.commandBufferInfoCount   = 1;
    info.pCommandBufferInfos      = cmd;

    return info;
}

