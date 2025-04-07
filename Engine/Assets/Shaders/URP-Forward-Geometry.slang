// ==================== Vertex Shader ====================

struct VertexInput
{
    float3 Position : POSITION;  // layout(location = 0)
    float3 Normal   : NORMAL;    // layout(location = 1)
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float3 Normal   : TEXCOORD0; // layout(location = 0) out
};

cbuffer FMatrixUBO : register(b0)
{
    float4x4 Projection;
    float4x4 Viewing;
    float4x4 Model;
}MatrixUBO;

[shader("vertex")]
VertexOutput VertexMain(VertexInput _Input)
{
    float4 WorldPos   = mul(float4(_Input.Position, 1.0), MatrixUBO.Model);
    float4 ViewingPos = mul(WorldPos, MatrixUBO.Viewing);
    float4 ClippedPos = mul(ViewingPos, MatrixUBO.Projection);

    VertexOutput Output;
    Output.Position = ClippedPos;
    Output.Normal   = _Input.Normal;
    return Output;
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