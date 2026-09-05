#include "game/tilemap/tilemap.h"

#include <engine/engine.h>
#include <engine/graphics/graphics.h>
#include <vk_mem_alloc.h>
#include <volk.h>

#include <algorithm>
#include <cstring>
#include <tracy/Tracy.hpp>


auto GraphicOverlay::initialise(engine::GraphicsEngine& engine,
                                const LogicMap& logic_map,
                                engine::GraphicsRegisteredPipelineDeletionQueue& deletion_queue) -> void {
    ZoneScoped;

    allocated_tile_count_ = logic_map.tiles_.size();
    const std::size_t alloc_size = std::max<std::size_t>(allocated_tile_count_ * sizeof(std::uint32_t), sizeof(std::uint32_t));
    tilemap_buffer_              = engine.allocate_buffer(alloc_size,
                                             VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                             VMA_MEMORY_USAGE_GPU_ONLY);
    deletion_queue.push(tilemap_buffer_);
    initialised_ = true;
    dirty_       = true;

    update(logic_map);
}

auto GraphicOverlay::update(const LogicMap& logic_map) -> void {
    ZoneScoped;

    if (!initialised_ || !dirty_) {
        return;
    }

    gpu_tiles_.resize(logic_map.tiles_.size());
    std::ranges::transform(logic_map.tiles_, gpu_tiles_.begin(), [](std::uint64_t tile_id) {
        return static_cast<std::uint32_t>(tile_id);
    });

    auto& graphics_engine       = engine::Engine::instance().graphics;
    const std::size_t copy_size = std::max<std::size_t>(gpu_tiles_.size() * sizeof(std::uint32_t), sizeof(std::uint32_t));

    graphics_engine.immediate_submit([&](VkCommandBuffer cmd) {
        engine::graphics::StagingBuffer staging_buffer{
          .allocation  = graphics_engine.allocate_buffer(copy_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY),
          .mapped_data = nullptr,
          .total_size  = copy_size};
        vmaMapMemory(graphics_engine.allocator, staging_buffer.allocation.allocation, &staging_buffer.mapped_data);
        std::memcpy(staging_buffer.mapped_data, gpu_tiles_.data(), gpu_tiles_.size() * sizeof(std::uint32_t));

        VkBufferCopy copy{};
        copy.dstOffset = 0;
        copy.srcOffset = 0;
        copy.size      = copy_size;
        vkCmdCopyBuffer(cmd, staging_buffer.allocation.buffer, tilemap_buffer_.buffer, 1, &copy);

        vmaUnmapMemory(graphics_engine.allocator, staging_buffer.allocation.allocation);
        graphics_engine.next_frame().write().get().deletion_queue.push(staging_buffer.allocation);
    });

    dirty_ = false;
}

auto GraphicOverlay::mark_dirty() -> void {
    dirty_ = true;
}

auto GraphicOverlay::tile_count() const -> std::size_t {
    return allocated_tile_count_;
}
