struct TilemapPushConstants {
    uint map_width;
    uint map_height;
    uint tile_size;
    uint _padding;
};

[[vk::push_constant]]
ConstantBuffer<TilemapPushConstants> constants;

StructuredBuffer<uint> tile_ids;

[format("rgba16f")]
RWTexture2D<float4> image;

float3 colour_from_tile_id(uint tile_id) {
    if (tile_id == 0) {
        return float3(0.08, 0.08, 0.10);
    }

    uint hash = tile_id;
    hash ^= hash >> 16;
    hash *= 0x7feb352d;
    hash ^= hash >> 15;
    hash *= 0x846ca68b;
    hash ^= hash >> 16;

    return float3(
        0.25 + float(hash & 0xff) / 255.0 * 0.75,
        0.25 + float((hash >> 8) & 0xff) / 255.0 * 0.75,
        0.25 + float((hash >> 16) & 0xff) / 255.0 * 0.75);
}

[shader("compute")]
[numthreads(16, 16, 1)]
void main(uint3 thread_id: SV_DispatchThreadID) {
    int2 texel_coord = int2(thread_id.xy);
    int2 image_size;
    image.GetDimensions(image_size.x, image_size.y);

    if (texel_coord.x >= image_size.x || texel_coord.y >= image_size.y) {
        return;
    }

    uint tile_x = uint(texel_coord.x) / constants.tile_size;
    uint tile_y = uint(texel_coord.y) / constants.tile_size;

    if (tile_x >= constants.map_width || tile_y >= constants.map_height) {
        return;
    }

    uint tile_index = tile_x + tile_y * constants.map_width;
    uint tile_id = tile_ids[tile_index];
    float3 colour = colour_from_tile_id(tile_id);

    // Draw lightweight grid lines to make the map readable while this is still
    // ID-colour debug rendering. The atlas/sprite lookup can replace this later
    // without changing the C++ data path.
    uint local_x = uint(texel_coord.x) % constants.tile_size;
    uint local_y = uint(texel_coord.y) % constants.tile_size;
    if (local_x == 0 || local_y == 0) {
        colour *= 0.35;
    }

    image.Store(texel_coord, float4(colour, 1.0));
}
