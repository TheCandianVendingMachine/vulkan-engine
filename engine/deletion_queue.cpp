#include "engine/deletion_queue.h"

auto ENGINE_NS::GraphicsPerFrameDeletionQueue::flush(VulkanDevice&, VmaAllocator) -> void {
}

auto ENGINE_NS::GraphicsMainDeletionQueue::flush(VulkanDevice& device, VmaAllocator allocator) -> void {
    draw_image_.destroy(device.device, allocator);
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(ImageAllocation allocation) -> void {
    draw_image_ = Deletion<ImageAllocation>(allocation, 0);
}
