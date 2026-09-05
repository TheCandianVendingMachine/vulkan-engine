#include "game/tilemap/tilemap.h"

#include <engine/engine.h>
#include <engine/graphics/graphics.h>
#include <vk_mem_alloc.h>
#include <volk.h>

#include <cstring>
#include <tracy/Tracy.hpp>


auto GraphicOverlay::update(const LogicMap& logic_map) -> void {
    ZoneScoped;

    auto& graphics_engine = engine::Engine::instance().graphics;
    engine::Engine::instance().graphics.immediate_submit([&](VkCommandBuffer cmd) {
        std::size_t alloc_size = logic_map.tiles_.size() * sizeof(std::uint64_t);
        engine::graphics::StagingBuffer staging_buffer{
          .allocation  = graphics_engine.allocate_buffer(alloc_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY),
          .mapped_data = nullptr,
          .total_size  = alloc_size};
        vmaMapMemory(graphics_engine.allocator, staging_buffer.allocation.allocation, &staging_buffer.mapped_data);
        std::memcpy(staging_buffer.mapped_data, logic_map.tiles_.data(), alloc_size);


        VkBufferCopy copy{};
        copy.dstOffset = 0;
        copy.srcOffset = 0;
        copy.size      = alloc_size;
        vkCmdCopyBuffer(cmd, staging_buffer.allocation.buffer, tilemap_buffer_.buffer, 1, &copy);

        vmaUnmapMemory(graphics_engine.allocator, staging_buffer.allocation.allocation);

        graphics_engine.next_frame().write().get().deletion_queue.push(staging_buffer.allocation);
    });
}