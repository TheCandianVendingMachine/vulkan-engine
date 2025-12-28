struct Vertex {
    float3 position;
    float uv_x;
    float3 normal;
    float uv_y;
    float4 colour;
};

struct CoarseVertex {
    float4 colour;
    float3 normal;
    float2 uv;
};

struct PushConstant {
    float4x4 render_matrix;
    Vertex* vertex_buffer;
};

// Output of the fragment shader
struct Fragment {
    float4 colour;
};

struct VertexStageOutput {
    CoarseVertex    coarse_vertex   : CoarseVertex;
    float4          sv_position     : SV_Position;
};

[shader("vertex")]
VertexStageOutput vertexMain(int vertex_id: SV_VertexID, uniform PushConstant push_constants)
{
    VertexStageOutput output;

    Vertex vertex = push_constants.vertex_buffer[vertex_id];

    float4 position = mul(push_constants.render_matrix, float4(vertex.position, 1.f));
    float4 colour = vertex.colour;

    output.coarse_vertex.colour = vertex.colour;
    output.coarse_vertex.normal = vertex.normal;
    output.coarse_vertex.uv = float2(vertex.uv_x, vertex.uv_y);
    output.sv_position = position;

    return output;
}

// Fragment Shader

[shader("fragment")]
Fragment pixelMain(
    CoarseVertex coarse_vertex : CoarseVertex) : SV_Target
{
    float4 colour = coarse_vertex.colour;

    Fragment output;
    output.colour = colour;
    return output;
}