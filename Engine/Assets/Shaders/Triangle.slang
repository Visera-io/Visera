// Slang shader code equivalent to the GLSL shader

// Slang requires explicit semantics for outputs
struct VSOutput
{
    float4 position : SV_Position;
    float3 color : TEXCOORD0;
};

// Define arrays using static constants
static const float3 Positions[3] =
{
    float3(3.0, 1.0, 0.0),
    float3(-1.0, -3.0, 0.0),
    float3(-1.0, 1.0, 0.0)
};

static const float3 Colors[3] =
{
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
};

// Vertex shader function
[shader("vertex")]
VSOutput VertMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    output.position = float4(Positions[vertexID % 3], 1.0);
    output.color = Colors[vertexID % 3];
    return output;
}

// Pixel shader function
[shader("fragment")]
float4 FragMain(VSOutput input) : SV_Target
{
    return float4(input.color, 1.0);
}
