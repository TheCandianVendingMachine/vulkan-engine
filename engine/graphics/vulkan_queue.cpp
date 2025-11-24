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

ENGINE_NS::VulkanQueue::VulkanQueue(const VulkanQueue& rhs) :
    queue_family_(rhs.queue_family_), queue_index_(rhs.queue_family_), max_queue_index_(rhs.max_queue_index_), type_(rhs.type_),
    device_(rhs.device_), associated_queue_(rhs.associated_queue_) {
}

ENGINE_NS::VulkanQueue::VulkanQueue(VulkanQueue&& rhs) noexcept :
    queue_family_(std::move(rhs.queue_family_)), queue_index_(std::move(rhs.queue_family_)),
    max_queue_index_(std::move(rhs.max_queue_index_)), type_(std::move(rhs.type_)), device_(std::move(rhs.device_)),
    associated_queue_(std::move(rhs.associated_queue_)) {
}

auto ENGINE_NS::VulkanQueue::operator=(const VulkanQueue& rhs) -> VulkanQueue& {
    if (&rhs != this) {
        this->queue_family_     = rhs.queue_family_;
        this->queue_index_      = rhs.queue_index_;
        this->max_queue_index_  = rhs.max_queue_index_;
        this->type_             = rhs.type_;
        this->device_           = rhs.device_;
        this->associated_queue_ = rhs.associated_queue_;
    }
    return *this;
}

auto ENGINE_NS::VulkanQueue::operator=(VulkanQueue&& rhs) noexcept -> VulkanQueue& {
    if (&rhs != this) {
        this->queue_family_     = std::move(rhs.queue_family_);
        this->queue_index_      = std::move(rhs.queue_index_);
        this->max_queue_index_  = std::move(rhs.max_queue_index_);
        this->type_             = std::move(rhs.type_);
        this->device_           = std::move(rhs.device_);
        this->associated_queue_ = std::move(rhs.associated_queue_);
    }
    return *this;
}

ENGINE_NS::VulkanQueue::VulkanQueue(std::uint32_t queue_family, std::uint32_t queue_index, std::uint32_t max_queue_index,
                                    VulkanQueueType type) :
    queue_family_(queue_family), queue_index_(queue_index), max_queue_index_(max_queue_index), type_(type) {
}
