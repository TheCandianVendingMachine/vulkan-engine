#include "engine/engine.h"
#include "engine/graphics/vulkan.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable
#include <cstdint>

auto ENGINE_NS::VulkanQueue::get() -> VkQueue {
    if (associated_queue_ == VK_NULL_HANDLE) {
        vkGetDeviceQueue(device_->device, queue_family_, queue_index_, &associated_queue_);
    }
    return associated_queue_;
}

auto ENGINE_NS::VulkanQueue::get() const -> VkQueue {
    if (associated_queue_ != VK_NULL_HANDLE) {
        return associated_queue_;
    }
    VkQueue queue = VK_NULL_HANDLE;
    vkGetDeviceQueue(device_->device, queue_family_, queue_index_, &queue);
    return queue;
}

ENGINE_NS::VulkanQueue::VulkanQueue(std::uint32_t queue_family, std::uint32_t queue_index, std::uint32_t max_queue_index,
                                    VulkanQueueType type) :
    queue_family_(queue_family), queue_index_(queue_index), max_queue_index_(max_queue_index), type_(type) {
}
