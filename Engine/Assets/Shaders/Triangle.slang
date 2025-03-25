// Slang shader code equivalent to the GLSL shader

// Slang requires explicit semantics for outputs
struct VSOutput
{

};

// Define arrays using static constants
static const float3 Positions[3] =
{
    float3(3.0, 1.0, 0.0),
    float3(-1.0, -3.0, 0.0),
    float3(-1.0, 1.0, 0.0)
};

// Vertex shader function
[shader("vertex")]
VSOutput VertMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    return output;
}

// Pixel shader function
[shader("fragment")]
float4 FragMain(texture2D<float4> PrevSubpassColor: register(t0)) : SV_Target
{
    float4 color = PrevSubpassColor.Sample(sampler, );
    // Swizzle Color Component to Swapchain Format: BGRA
    return float4(input.color.bgr, 1.0);
}
