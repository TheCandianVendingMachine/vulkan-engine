static const float3 POSITIONS[3] = {
    float3(1.f, 1.f, 0.f),
    float3(-1.f, 1.f, 0.f),
    float3(0.f, -1.f, 0.f)
};
static const float3 COLOURS[3] = {
    float3(1.f, 0.f, 0.f),
    float3(0.f, 1.f, 0.f),
    float3(0.f, 0.f, 1.f)
};

// Output of the vertex shader, and input to the fragment shader.
struct CoarseVertex
{
    float3 colour;
};

// Output of the fragment shader
struct Fragment
{
    float4 colour;
};

struct VertexStageOutput
{
    CoarseVertex    coarseVertex    : CoarseVertex;
    float4          sv_position     : SV_Position;
};

[shader("vertex")]
VertexStageOutput vertexMain(int vertex_id: SV_VertexID)
{
    VertexStageOutput output;

    float3 position = POSITIONS[vertex_id];
    float3 colour    = COLOURS[vertex_id];

    output.coarseVertex.colour = colour;
    output.sv_position = float4(position, 1.0);

    return output;
}

// Fragment Shader

[shader("fragment")]
Fragment pixelMain(
    CoarseVertex coarseVertex : CoarseVertex) : SV_Target
{
    float3 colour = coarseVertex.colour;

    Fragment output;
    output.colour = float4(colour, 1.0);
    return output;
}