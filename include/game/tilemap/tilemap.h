#pragma once

#include <cstdint>
#include <engine/graphics/descriptor.h>
#include <engine/graphics/types.h>
#include <engine/graphics/vulkan.h>
#include <engine/reflection/type.h>
#include <engine/state/state.h>
#include <linalg/vector.h>
#include <robin_map.h>
#include <string>
#include <vector>
#include <vk_mem_alloc.h>

struct Tile {
        std::string tile_name{};
        std::string asset_name{};
        std::string collision_layer{};

        auto id() const -> std::size_t;

        REFLECT_START(Tile)
        REFLECT_MEMBER(tile_name), REFLECT_MEMBER(asset_name), REFLECT_MEMBER(collision_layer) REFLECT_END
};
template <>
struct std::hash<Tile> {
        auto operator()(const Tile& tile) const noexcept -> std::size_t {
            return std::hash<std::string>{}(tile.tile_name);
        }
};

class LogicMap {
    public:
        LogicMap(std::uint64_t size_x, std::uint64_t size_y, std::uint64_t tile_size, Tile null_tile);

        auto set(linalg::Vector2<std::uint64_t> position, Tile tile) -> void;
        auto get(linalg::Vector2<std::uint64_t> position) const -> std::uint64_t;

        auto get(linalg::Vector2<double> position) const -> std::uint64_t;
        auto get_nearby(linalg::Vector2<double> position, double radius) const -> std::vector<std::uint64_t>;

    private:
        auto index_from_coordinates_(linalg::Vector2<double> coordinate) const -> std::size_t;
        auto index_from_coordinates_(linalg::Vector2<std::uint64_t> coordinate) const -> std::size_t;

        std::uint64_t size_x_    = 0;
        std::uint64_t size_y_    = 0;
        std::uint64_t tile_size_ = 0;

        Tile null_tile_{};
        std::vector<std::uint64_t> tiles_{};
};

class GraphicOverlay {
    public:
    private:
};

class TileMap {
    public:
        TileMap(std::uint64_t size_x, std::uint64_t size_y, std::uint64_t tile_size, Tile null_tile);

        auto set_position(linalg::Vector2<double> position);

        auto set(linalg::Vector2<std::uint64_t> position, Tile tile) -> void;
        auto get(linalg::Vector2<std::uint64_t> position) -> Tile;

        auto get(linalg::Vector2<double> position) -> Tile;
        auto get_nearby(linalg::Vector2<double> position, double radius) -> std::vector<Tile>;

        auto relative_position(linalg::Vector2<double> world_position) const -> linalg::Vector2<double>;

        const std::uint64_t size_x;
        const std::uint64_t size_y;
        const std::uint64_t tile_size;

    private:
        tsl::robin_map<std::uint64_t, Tile> hash_to_tile_;
        linalg::Vector2<double> position_;

        LogicMap logic_;
        GraphicOverlay graphics_;
};


class TilemapPreDrawPipeline : public engine::StatePipeline {
    public:
        using engine::StatePipeline::StatePipeline;

        virtual auto name() const -> std::string override final;
        virtual auto build_compute_pipeline(engine::GraphicsEngine& engine,
                                            engine::VulkanDevice& device,
                                            engine::GraphicsRegisteredPipelineDeletionQueue& pipeline_deletion_queue)
            -> std::optional<engine::ComputePipelineBuilder> override final;

    protected:
        virtual auto record_compute_(VkCommandBuffer cmd) -> void override final;


    private:
        auto create_descriptors_(engine::GraphicsEngine& engine,
                                 engine::VulkanDevice& device,
                                 engine::GraphicsRegisteredPipelineDeletionQueue& pipeline_deletion_queue) -> void;

        VkDescriptorSet tilemap_id_image_descriptors_ = VK_NULL_HANDLE;
        engine::VulkanDescriptorSetLayout tilemap_id_image_layout_{};

        engine::ImageAllocation tilemap_id_image_{};
};

