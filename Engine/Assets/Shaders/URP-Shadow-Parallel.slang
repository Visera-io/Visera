// ==================== Vertex Shader ====================

struct VertexInput
{
    float3 Position : POSITION;     // layout(location = 0)
};

struct VertexOutput
{
    float4 Position : SV_Position;
};

cbuffer FMatrixUBO : register(b0)
{
    float4x4 MVP;
}MatrixUBO;

[shader("vertex")]
VertexOutput VertexMain(VertexInput _Input)
{
    float4 ClipPos = mul(float4(_Input.Position, 1.0), MatrixUBO.MVP);

    VertexOutput Output;
    Output.Position = ClipPos;
    return Output;
}

// ==================== Fragment Shader ====================
[shader("fragment")]
void FragmentMain()
{
    return; // Depth Only
}