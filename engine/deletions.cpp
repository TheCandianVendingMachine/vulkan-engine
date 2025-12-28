#include "engine/deletion_queue.h"
#include "engine/graphics/types.h"

#include <cstdint>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <vk_mem_alloc.h>
#include <volk.h>
#include <vulkan/vulkan_core.h>
// clang-format: off
#include <Tracy/TracyVulkan.hpp>
// clang-format: on

auto ENGINE_NS::Deletion<ENGINE_NS::ImageAllocation>::destroy(VkDevice device, VmaAllocator allocator) -> void {
    vkDestroyImageView(device, object.view, nullptr);
    vmaDestroyImage(allocator, object.image, object.allocation);
}

auto ENGINE_NS::Deletion<ENGINE_NS::BufferAllocation>::destroy(VkDevice, VmaAllocator allocator) -> void {
    vmaDestroyBuffer(allocator, object.buffer, object.allocation);
}

auto ENGINE_NS::Deletion<ENGINE_NS::GPUMeshBuffers>::destroy(VkDevice device, VmaAllocator allocator) -> void {
    Deletion<BufferAllocation>(object.index_buffer, 0).destroy(device, allocator);
    Deletion<BufferAllocation>(object.vertex_buffer, 0).destroy(device, allocator);
}

auto ENGINE_NS::Deletion<ENGINE_NS::VulkanDescriptorSetLayout>::destroy(VkDevice device) -> void {
    vkDestroyDescriptorSetLayout(device, object.layout, nullptr);
}

auto ENGINE_NS::Deletion<ENGINE_NS::ComputePipeline>::destroy(VkDevice device) -> void {
    vkDestroyPipeline(device, object.pipeline, nullptr);
    vkDestroyPipelineLayout(device, object.pipeline_layout, nullptr);
}

auto ENGINE_NS::Deletion<ENGINE_NS::GraphicsPipeline>::destroy(VkDevice device) -> void {
    vkDestroyPipeline(device, object.pipeline, nullptr);
    vkDestroyPipelineLayout(device, object.layout, nullptr);
}

auto ENGINE_NS::Deletion<ENGINE_NS::asset::CompiledShader>::destroy(VkDevice device) -> void {
    vkDestroyShaderModule(device, object.shader, nullptr);
}

auto ENGINE_NS::Deletion<ENGINE_NS::graphics::ImGui>::destroy(VkDevice device) -> void {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(device, object.descriptor_pool, nullptr);
}

auto ENGINE_NS::Deletion<ENGINE_NS::graphics::Immediate>::destroy(VkDevice device) -> void {
    constexpr std::uint64_t TIMEOUT = 1'000'000'000;
    vkWaitForFences(device, 1, &object.fence, VK_TRUE, TIMEOUT);
    vkDestroyFence(device, object.fence, nullptr);
    if (object.tracy_context) {
        TracyVkDestroy(object.tracy_context);
    }
    vkDestroyCommandPool(device, object.command_pool, nullptr);
}
