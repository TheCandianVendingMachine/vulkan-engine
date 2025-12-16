[format("rgba16f")]
RWTexture2D<vector<float, 4>> image;

[shader("compute")]
[numthreads(16, 16, 1)]
void computeMain(uint3 thread_id: SV_DispatchThreadID) {
    vector<int, 2> texel_coord = thread_id.xy;
    vector<int, 2> size;
    image.GetDimensions(size.x, size.y);

    if (texel_coord.x < size.x && texel_coord.y < size.y) {
        vector<float, 4> colour;
        colour.x = float(texel_coord.x) / float(size.x);
        colour.y = float(texel_coord.y) / float(size.y);

        image.Store(texel_coord, colour);
    }
}
