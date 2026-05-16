[format("rgba16f")]
RWTexture2D<float4> image;

[shader("compute")]
[numthreads(16, 16, 1)]
void computeMain(uint3 thread_id: SV_DispatchThreadID, uint3 local_id: SV_GroupThreadID) {
    vector<int, 2> texel_coord = thread_id.xy;
    vector<int, 2> size;
    image.GetDimensions(size.x, size.y);

    if (texel_coord.x < size.x && texel_coord.y < size.y) {
        float4 colour = float4(0);

        if (local_id.x != 0 && local_id.y != 0) {
            colour.x = float(texel_coord.x) / float(size.x);
            colour.y = float(texel_coord.y) / float(size.y);
        }

        image.Store(texel_coord, colour);
    }
}
