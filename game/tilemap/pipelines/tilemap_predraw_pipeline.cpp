#include "engine/state/state.h"
#include "game/tilemap/tilemap.h"

#include <engine/assets/library.h>
#include <engine/engine.h>
#include <engine/engine_utils.h>
#include <engine/graphics/descriptor.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/pipeline.h>
#include <engine/graphics/vulkan.h>

#include <algorithm>
#include <optional>
#include <tracy/Tracy.hpp>

// This is currently a compute-only tilemap renderer. It writes directly into the
// engine draw image so game code gets visible tiles without needing a second draw
// pass. Later this can become a true pre-draw pass by writing to an intermediate
// tilemap image and sampling/blending it in a regular graphics pipeline.
TilemapDrawPipeline::TilemapDrawPipeline(engine::State& state, TileMap& tilemap) :
    engine::StatePipeline(state), tilemap_(tilemap) {
    push_constants_.map_width  = static_cast<std::uint32_t>(tilemap_.size_x);
    push_constants_.map_height = static_cast<std::uint32_t>(tilemap_.size_y);
    push_constants_.tile_size  = static_cast<std::uint32_t>(tilemap_.tile_size);
}

auto TilemapDrawPipeline::name() const -> std::string {
    return "Tilemap [compute draw]";
}

auto TilemapDrawPipeline::build_compute_pipeline(engine::GraphicsEngine& engine,
                                                 engine::VulkanDevice& device,
                                                 engine::GraphicsRegisteredPipelineDeletionQueue& initialisation_deletion_queue)
    -> std::optional<engine::ComputePipelineBuilder> {
    ZoneScoped;

    tilemap_.graphics_.initialise(engine, tilemap_.logic_, deletion_queue_);
    create_descriptors_(engine, device, deletion_queue_);

    auto shader_result = engine::asset::BytecodeShader::load_from_file("assets/shaders/game/tilemap/tilemap.spv").compile(device);
    if (!shader_result.has_value()) {
        engine::crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__);
        std::unreachable();
    }
    auto shader = shader_result.value();

    VkPushConstantRange push_constant_range{};
    push_constant_range.offset     = 0;
    push_constant_range.size       = sizeof(TilemapPushConstants);
    push_constant_range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    auto tilemap_pipeline = std::move(engine::ComputePipeline::build()
                                        .layout()
                                        .add_set_layout(tilemap_id_image_layout_)
                                        .push_constant_range(push_constant_range)
                                        .finish()
                                        .shader(shader));
    initialisation_deletion_queue.push(shader);

    return tilemap_pipeline;
}

auto TilemapDrawPipeline::record_compute_(VkCommandBuffer cmd) -> void {
    tilemap_.graphics_.update(tilemap_.logic_);

    vkCmdBindDescriptorSets(cmd,
                            VK_PIPELINE_BIND_POINT_COMPUTE,
                            compute_pipeline_->layout,
                            0,
                            1,
                            &tilemap_id_image_descriptors_,
                            0,
                            nullptr);

    const std::uint32_t group_count_x = (engine::Engine::instance().graphics.draw_image.extent.width + 15) / 16;
    const std::uint32_t group_count_y = (engine::Engine::instance().graphics.draw_image.extent.height + 15) / 16;
    vkCmdDispatch(cmd, std::max(1u, group_count_x), std::max(1u, group_count_y), 1);
}

auto TilemapDrawPipeline::push_constants() -> engine::GPUPushConstants {
    return engine::GPUPushConstants{.data = &push_constants_, .size = sizeof(push_constants_)};
}

auto TilemapDrawPipeline::create_descriptors_(engine::GraphicsEngine& engine,
                                              engine::VulkanDevice& device,
                                              engine::GraphicsRegisteredPipelineDeletionQueue& pipeline_deletion_queue) -> void {
    ZoneScoped;
    tilemap_id_image_layout_ = engine::VulkanDescriptorSetLayout::build()
                                 .with_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                                 .with_binding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
                                 .build(device, VK_SHADER_STAGE_COMPUTE_BIT, nullptr, 0);

    tilemap_id_image_descriptors_ = pipeline_descriptor_allocator_.allocate(device, tilemap_id_image_layout_.layout);
    engine::DescriptorWriter{}
        .write_buffer(engine::Binding(0),
                      tilemap_.graphics_.tilemap_buffer_.buffer,
                      tilemap_.graphics_.tile_count() * sizeof(std::uint32_t),
                      0,
                      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
        .write_image(engine::Binding(1), engine.draw_image.view, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
        .update_set(device, tilemap_id_image_descriptors_);

    pipeline_deletion_queue.push(tilemap_id_image_layout_);
}
