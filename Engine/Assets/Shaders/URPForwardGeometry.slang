// ==================== Vertex Shader ====================

struct VertexInput
{
    float3 position : POSITION;  // layout(location = 0)
    float3 normal   : NORMAL;    // layout(location = 1)
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 normal   : TEXCOORD0; // layout(location = 0) out
};

[shader("vertex")]
VertexOutput VertexMain(VertexInput input)
{
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    output.normal = input.normal;
    return output;
}

// ==================== Fragment Shader ====================

struct FragmentInput
{
    float3 normal : TEXCOORD0; // Matches vertex output
};

[shader("fragment")]
float4 FragmentMain(FragmentInput input) : SV_TARGET
{
    return float4(input.normal, 1.0); // Use interpolated normal as color
}