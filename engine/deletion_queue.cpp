#include "engine/deletion_queue.h"

#include "engine/graphics/pipeline.h"
#include "engine/graphics/vulkan.h"

auto ENGINE_NS::GraphicsPerFrameDeletionQueue::flush(VulkanDevice& device, VmaAllocator allocator) -> void {
    for (auto& image : images_) {
        image.destroy(device.device, allocator);
    }
    for (auto& buffer : buffers_) {
        buffer.destroy(device.device, allocator);
    }
    for (auto& layout : layouts_) {
        layout.destroy(device.device);
    }
    for (auto& mesh_buffer : mesh_buffers_) {
        mesh_buffer.destroy(device.device, allocator);
    }
    for (auto& shader : shaders_) {
        shader.destroy(device.device);
    }
    images_.clear();
    buffers_.clear();
    layouts_.clear();
    mesh_buffers_.clear();
    shaders_.clear();
    index_ = 0;
}

auto ENGINE_NS::GraphicsMainDeletionQueue::flush(VulkanDevice& device, VmaAllocator allocator) -> void {
    draw_image_.destroy(device.device, allocator);
    for (auto& layout : layouts_) {
        layout.destroy(device.device);
    }
    for (auto& pipeline : compute_pipelines_) {
        pipeline.destroy(device.device);
    }
    for (auto& pipeline : graphics_pipelines_) {
        pipeline.destroy(device.device);
    }
    for (auto& mesh_buffer : mesh_buffers_) {
        mesh_buffer.destroy(device.device, allocator);
    }
    for (auto& immediate : immediates_) {
        immediate.destroy(device.device);
    }
    imgui_.destroy(device.device);
}

auto ENGINE_NS::GraphicsUploadDeletionQueue::flush(VulkanDevice& device, VmaAllocator allocator) -> void {
    for (auto& buffer : buffers_) {
        buffer.destroy(device.device, allocator);
    }
    buffers_.clear();
    index_ = 0;
}

auto ENGINE_NS::GraphicsPerFrameDeletionQueue::push(ImageAllocation& allocation) -> void {
    allocation.will_be_destroyed_ = true;
    images_.push_back(Deletion<ImageAllocation>(allocation, index_++));
}

auto ENGINE_NS::GraphicsPerFrameDeletionQueue::push(BufferAllocation& allocation) -> void {
    allocation.will_be_destroyed_ = true;
    buffers_.push_back(Deletion<BufferAllocation>(allocation, index_++));
}

auto ENGINE_NS::GraphicsPerFrameDeletionQueue::push(VulkanDescriptorSetLayout layout) -> void {
    layouts_.push_back(Deletion<VulkanDescriptorSetLayout>(layout, index_++));
}

auto ENGINE_NS::GraphicsPerFrameDeletionQueue::push(GPUMeshBuffers& buffers) -> void {
    buffers.index_buffer.will_be_destroyed_  = true;
    buffers.vertex_buffer.will_be_destroyed_ = true;
    mesh_buffers_.push_back(Deletion<GPUMeshBuffers>(buffers, index_++));
}

auto ENGINE_NS::GraphicsPerFrameDeletionQueue::push(asset::CompiledShader& shader) -> void {
    shaders_.push_back(Deletion<asset::CompiledShader>(shader, 0));
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(ImageAllocation& allocation) -> void {
    allocation.will_be_destroyed_ = true;
    draw_image_                   = Deletion<ImageAllocation>(allocation, 0);
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(VulkanDescriptorSetLayout layout) -> void {
    layouts_.push_back(Deletion<VulkanDescriptorSetLayout>(layout, 0));
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(ComputePipeline pipeline) -> void {
    compute_pipelines_.push_back(Deletion<ComputePipeline>(pipeline, 0));
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(GraphicsPipeline pipeline) -> void {
    graphics_pipelines_.push_back(Deletion<GraphicsPipeline>(pipeline, 0));
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(graphics::ImGui imgui) -> void {
    imgui_ = Deletion<graphics::ImGui>(imgui, 0);
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(GPUMeshBuffers& buffers) -> void {
    buffers.index_buffer.will_be_destroyed_  = true;
    buffers.vertex_buffer.will_be_destroyed_ = true;
    mesh_buffers_.push_back(Deletion<GPUMeshBuffers>(buffers, 0));
}

auto ENGINE_NS::GraphicsMainDeletionQueue::push(graphics::Immediate immediate) -> void {
    immediates_.push_back(Deletion<graphics::Immediate>(immediate, 0));
}

auto ENGINE_NS::GraphicsUploadDeletionQueue::push(BufferAllocation& allocation) -> void {
    allocation.will_be_destroyed_ = true;
    buffers_.push_back(Deletion<BufferAllocation>(allocation, index_++));
}

auto ENGINE_NS::GraphicsRegisteredPipelineDeletionQueue::flush(VulkanDevice& device, VmaAllocator allocator) -> void {
    for (auto& layout : layouts_) {
        layout.destroy(device.device);
    }
    for (auto& image : images_) {
        image.destroy(device.device, allocator);
    }
    for (auto& pipeline : compute_pipelines_) {
        pipeline.destroy(device.device);
    }
    for (auto& pipeline : graphics_pipelines_) {
        pipeline.destroy(device.device);
    }
    for (auto& mesh_buffer : mesh_buffers_) {
        mesh_buffer.destroy(device.device, allocator);
    }
    for (auto& immediate : immediates_) {
        immediate.destroy(device.device);
    }

    layouts_.clear();
    images_.clear();
    compute_pipelines_.clear();
    graphics_pipelines_.clear();
    mesh_buffers_.clear();
    immediates_.clear();
    index_ = 0;
}

auto ENGINE_NS::GraphicsRegisteredPipelineDeletionQueue::push(ImageAllocation& allocation) -> void {
    allocation.will_be_destroyed_ = true;
    images_.push_back(Deletion<ImageAllocation>(allocation, 0));
}

auto ENGINE_NS::GraphicsRegisteredPipelineDeletionQueue::push(VulkanDescriptorSetLayout layout) -> void {
    layouts_.push_back(Deletion<VulkanDescriptorSetLayout>(layout, 0));
}

auto ENGINE_NS::GraphicsRegisteredPipelineDeletionQueue::push(ComputePipeline pipeline) -> void {
    compute_pipelines_.push_back(Deletion<ComputePipeline>(pipeline, 0));
}

auto ENGINE_NS::GraphicsRegisteredPipelineDeletionQueue::push(GraphicsPipeline pipeline) -> void {
    graphics_pipelines_.push_back(Deletion<GraphicsPipeline>(pipeline, 0));
}

auto ENGINE_NS::GraphicsRegisteredPipelineDeletionQueue::push(GPUMeshBuffers& buffers) -> void {
    buffers.index_buffer.will_be_destroyed_  = true;
    buffers.vertex_buffer.will_be_destroyed_ = true;
    mesh_buffers_.push_back(Deletion<GPUMeshBuffers>(buffers, index_++));
}

auto ENGINE_NS::GraphicsRegisteredPipelineDeletionQueue::push(graphics::Immediate immediate) -> void {
    immediates_.push_back(Deletion<graphics::Immediate>(immediate, 0));
}
