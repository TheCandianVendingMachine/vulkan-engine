#include "engine/deletion_queue.h"

auto ENGINE_NS::GraphicsPerFrameDeletionQueue::flush(VulkanDevice&, VmaAllocator) -> void {
}

auto ENGINE_NS::GraphicsMainDeletionQueue::flush(VulkanDevice& device, VmaAllocator allocator) -> void {
    draw_image_.destroy(device.device, allocator);
    for (auto& layout : layouts_) {
        layout.destroy(device.device);
    }
    for (auto& pipeline : compute_pipelines_) {
        pipeline.destroy(device.device);
    }
    imgui_.destroy(device.device);
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(ImageAllocation allocation) -> void {
    draw_image_ = Deletion<ImageAllocation>(allocation, 0);
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(VulkanDescriptorSetLayout layout) -> void {
    layouts_.push_back(Deletion<VulkanDescriptorSetLayout>(layout, 0));
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(ComputePipeline pipeline) -> void {
    compute_pipelines_.push_back(Deletion<ComputePipeline>(pipeline, 0));
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(graphics::ImGui imgui) -> void {
    imgui_ = Deletion<graphics::ImGui>(imgui, 0);
}

