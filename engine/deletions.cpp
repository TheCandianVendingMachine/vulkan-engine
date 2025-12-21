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

auto ENGINE_NS::Deletion<ENGINE_NS::VulkanDescriptorSetLayout>::destroy(VkDevice device) -> void {
    vkDestroyDescriptorSetLayout(device, object.layout, nullptr);
}

auto ENGINE_NS::Deletion<ENGINE_NS::ComputePipeline>::destroy(VkDevice device) -> void {
    vkDestroyPipeline(device, object.pipeline, nullptr);
    vkDestroyPipelineLayout(device, object.pipeline_layout, nullptr);
}

auto ENGINE_NS::Deletion<ENGINE_NS::asset::CompiledShader>::destroy(VkDevice device) -> void {
    vkDestroyShaderModule(device, object.shader, nullptr);
}
