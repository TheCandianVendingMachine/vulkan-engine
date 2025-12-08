#include "engine/deletion_queue.h"
#include "engine/graphics/types.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable
#include <vk_mem_alloc.h>

auto ENGINE_NS::Deletion<ENGINE_NS::ImageAllocation>::destroy(VkDevice device, VmaAllocator allocator) -> void {
    vkDestroyImageView(device, object.view, nullptr);
    vmaDestroyImage(allocator, object.image, object.allocation);
}
