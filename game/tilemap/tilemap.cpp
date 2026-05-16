#include "game/tilemap/tilemap.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <engine/assets/library.h>
#include <engine/deletion_queue.h>
#include <engine/engine_utils.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/pipeline.h>
#include <engine/graphics/vulkan.h>
#include <engine/meta_defines.h>
#include <iterator>
#include <linalg/vector.h>
#include <numbers>
#include <ranges>
#include <robin_set.h>
#include <string>
#include <tracy/Tracy.hpp>
#include <utility>
#include <vector>

TileMap::TileMap(std::uint64_t size_x, std::uint64_t size_y, std::uint64_t tile_size, Tile null_tile) :
    size_x(size_x), size_y(size_y), tile_size(tile_size), logic_(LogicMap(size_x, size_y, tile_size, null_tile)) {
}

auto TileMap::set_position(linalg::Vector2<double> position) {
    position_ = position;
}

auto TileMap::set(linalg::Vector2<std::uint64_t> position, Tile tile) -> void {
    if (!hash_to_tile_.contains(tile.id())) {
        hash_to_tile_.insert({tile.id(), tile});
    }
    logic_.set(position, tile);
}

auto TileMap::get(linalg::Vector2<std::uint64_t> position) -> Tile {
    auto tile_id = logic_.get(position);
    return hash_to_tile_.at(tile_id);
}

auto TileMap::get(linalg::Vector2<double> position) -> Tile {
    auto tile_id = logic_.get(relative_position(position));
    return hash_to_tile_.at(tile_id);
}

auto TileMap::get_nearby(linalg::Vector2<double> position, double radius) -> std::vector<Tile> {
    auto tile_ids = logic_.get_nearby(relative_position(position), radius);
    std::vector<Tile> tiles;
    std::ranges::copy(std::views::transform(tile_ids,
                                            [&](std::uint64_t tile_id) {
                                                return hash_to_tile_.at(tile_id);
                                            }),
                      std::back_inserter(tiles));
    return tiles;
}

auto TileMap::relative_position(linalg::Vector2<double> world_position) const -> linalg::Vector2<double> {
    return world_position - position_;
}

LogicMap::LogicMap(std::uint64_t size_x, std::uint64_t size_y, std::uint64_t tile_size, Tile null_tile) :
    size_x_(size_x), size_y_(size_y), tile_size_(tile_size), null_tile_(null_tile) {
    tiles_.resize(size_x_ * size_y_, null_tile_.id());
}

auto LogicMap::set(linalg::Vector2<std::uint64_t> position, Tile tile) -> void {
    tiles_.at(index_from_coordinates_(position)) = tile.id();
}

auto LogicMap::get(linalg::Vector2<std::uint64_t> position) const -> std::uint64_t {
    return tiles_.at(index_from_coordinates_(position));
}

auto LogicMap::get(linalg::Vector2<double> position) const -> std::uint64_t {
    return tiles_.at(index_from_coordinates_(linalg::Vector2<std::uint64_t>{static_cast<std::uint64_t>(position.x / tile_size_),
                                                                            static_cast<std::uint64_t>(position.y / tile_size_)}));
}

auto LogicMap::get_nearby(linalg::Vector2<double> position, double radius) const -> std::vector<std::uint64_t> {
    std::vector<std::uint64_t> tiles;
    double scaled_radius = radius / tile_size_;
    auto area            = static_cast<std::size_t>(std::ceil(std::numbers::pi * scaled_radius * scaled_radius));
    tiles.reserve(area);

    std::vector<linalg::Vector2<std::uint64_t>> frontier{};
    frontier.reserve(static_cast<std::size_t>(2.0 * std::numbers::pi * radius));
    frontier.emplace_back(linalg::Vector2<std::uint64_t>::zero());

    tsl::robin_set<linalg::Vector2<std::uint64_t>> seen{};
    seen.reserve(area);
    seen.insert(frontier[0]);

    while (!frontier.empty()) {
        linalg::Vector2<std::uint64_t> front = frontier[0];
        frontier.erase(frontier.begin());

        auto distance = static_cast<double>(front.x * front.x + front.y * front.y);
        if (distance > std::ceil(radius * radius)) {
            continue;
        }

        linalg::Vector2<double> real_coordinate{position.x + static_cast<double>(front.x * tile_size_),
                                                position.y + static_cast<double>(front.y * tile_size_)};
        tiles.emplace_back(tiles_[index_from_coordinates_(real_coordinate)]);

        for (auto& considering : std::array<linalg::Vector2<std::uint64_t>, 4>{
               linalg::Vector2<std::uint64_t>{front.x + 1, front.y    },
               linalg::Vector2<std::uint64_t>{front.x - 1, front.y    },
               linalg::Vector2<std::uint64_t>{front.x,     front.y + 1},
               linalg::Vector2<std::uint64_t>{front.x,     front.y - 1},
        }) {
            if (!seen.contains(considering)) {
                frontier.emplace_back(considering);
                seen.insert({considering});
            }
        }
    }

    return tiles;
}

auto LogicMap::index_from_coordinates_(linalg::Vector2<double> coordinate) const -> std::size_t {
    linalg::Vector2<double> scaled_coordinate                = coordinate / static_cast<double>(tile_size_);
    linalg::Vector2<std::uint64_t> scaled_integer_coordinate = {static_cast<std::uint64_t>(std::floor(scaled_coordinate.x)),
                                                                static_cast<std::uint64_t>(std::floor(scaled_coordinate.y))};
    return index_from_coordinates_(scaled_integer_coordinate);
}

auto LogicMap::index_from_coordinates_(linalg::Vector2<std::uint64_t> coordinate) const -> std::size_t {
    return static_cast<std::size_t>(coordinate.x + size_x_ * coordinate.y);
}

auto Tile::id() const -> std::size_t {
    return std::hash<Tile>{}(*this);
}

auto TilemapPipeline::name() const -> std::string {
    return "Tilemap";
}

auto TilemapPipeline::build_pipeline(engine::GraphicsEngine& engine,
                                     engine::VulkanDevice& device,
                                     engine::GraphicsRegisteredPipelineDeletionQueue&) -> engine::GraphicsPipelineBuilder {
    ZoneScoped;
    auto shader_result = engine::asset::BytecodeShader::load_from_file("assets/shaders/game/tilemap/tilemap.spv").compile(device);
    if (!shader_result.has_value()) {
        engine::crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__);
        std::unreachable();
    }
    auto shader            = shader_result.value();

    auto triangle_pipeline = engine::GraphicsPipeline::build()
                                 .layout()
                                 .finish()
                                 .shaders(shader, shader)
                                 .input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                                 .polygon_mode(VK_POLYGON_MODE_FILL)
                                 .cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
                                 .set_multisampling_none()
                                 .disable_blending()
                                 .disable_depthtest()
                                 //.color_attachment_format(engine)
                                 .depth_format(VK_FORMAT_UNDEFINED);
    engine.destroy_shader(shader);

    return triangle_pipeline;
}

auto TilemapPipeline::record_(VkCommandBuffer) -> void {
}

auto TilemapDebugPipeline::name() const -> std::string {
    return "Tilemap [Debug]";
}

auto TilemapDebugPipeline::build_pipeline(engine::GraphicsEngine& engine,
                                          engine::VulkanDevice& device,
                                          engine::GraphicsRegisteredPipelineDeletionQueue&) -> engine::GraphicsPipelineBuilder {
    ZoneScoped;
    auto shader_result = engine::asset::BytecodeShader::load_from_file("assets/shaders/game/tilemap/tilemap_debug.spv").compile(device);
    if (!shader_result.has_value()) {
        engine::crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__);
        std::unreachable();
    }
    auto shader            = shader_result.value();

    auto triangle_pipeline = engine::GraphicsPipeline::build()
                                 .layout()
                                 .finish()
                                 .shaders(shader, shader)
                                 .input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                                 .polygon_mode(VK_POLYGON_MODE_FILL)
                                 .cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
                                 .set_multisampling_none()
                                 .disable_blending()
                                 .disable_depthtest()
                                 //.color_attachment_format(engine)
                                 .depth_format(VK_FORMAT_UNDEFINED);
    engine.destroy_shader(shader);

    return triangle_pipeline;
}

auto TilemapDebugPipeline::record_(VkCommandBuffer) -> void {
}
