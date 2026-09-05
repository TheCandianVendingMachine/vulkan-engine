#include "engine/state/state.h"
#include "game/tilemap/tilemap.h"

TilemapPreDrawPipeline::TilemapPreDrawPipeline(engine::State& state, TileMap& tilemap) :
    engine::StatePipeline(state), tilemap_(tilemap.graphics_) {
}

auto TilemapPreDrawPipeline::name() const -> std::string {
    return "Tilemap [pre-draw]";
}

auto TilemapPreDrawPipeline::build_compute_pipeline(engine::GraphicsEngine& engine,
                                                    engine::VulkanDevice& device,
                                                    engine::GraphicsRegisteredPipelineDeletionQueue& initialisation_deletion_queue)
    -> std::optional<engine::ComputePipelineBuilder> {
    ZoneScoped;
    tilemap_id_image_ =
        engine.allocate_image(VkExtent3D{.width = 64, .height = 64, .depth = 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_STORAGE_BIT);
    deletion_queue_.push(tilemap_id_image_);

    create_descriptors_(engine, device, deletion_queue_);

    auto shader_result = engine::asset::BytecodeShader::load_from_file("assets/shaders/game/tilemap/tilemap.spv").compile(device);
    if (!shader_result.has_value()) {
        engine::crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__);
        std::unreachable();
    }
    auto shader = shader_result.value();

    auto triangle_pipeline =
        std::move(engine::ComputePipeline::build().layout().add_set_layout(tilemap_id_image_layout_).finish().shader(shader));
    initialisation_deletion_queue.push(shader);

    return triangle_pipeline;
}

auto TilemapPreDrawPipeline::record_compute_(VkCommandBuffer cmd) -> void {
    vkCmdBindDescriptorSets(cmd,
                            VK_PIPELINE_BIND_POINT_COMPUTE,
                            compute_pipeline_->layout,
                            0,
                            1,
                            &tilemap_id_image_descriptors_,
                            0,
                            nullptr);
    vkCmdDispatch(cmd, 80, 45, 1);
}

auto TilemapPreDrawPipeline::create_descriptors_(engine::GraphicsEngine&,
                                                 engine::VulkanDevice& device,
                                                 engine::GraphicsRegisteredPipelineDeletionQueue& pipeline_deletion_queue) -> void {
    ZoneScoped;
    tilemap_id_image_layout_ = engine::VulkanDescriptorSetLayout::build()
                                   .with_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
                                   .build(device, VK_SHADER_STAGE_COMPUTE_BIT, nullptr, 0);

    tilemap_id_image_descriptors_ = pipeline_descriptor_allocator_.allocate(device, tilemap_id_image_layout_.layout);
    engine::DescriptorWriter{}
        .write_image(engine::Binding(0), tilemap_id_image_.view, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
        .update_set(device, tilemap_id_image_descriptors_);

    pipeline_deletion_queue.push(tilemap_id_image_layout_);
}
