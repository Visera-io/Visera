// ==================== Vertex Shader ====================

struct VertexInput
{
    float3 Position : POSITION;  // layout(location = 0)
    float3 Normal   : NORMAL;    // layout(location = 1)
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal   : TEXCOORD0; // layout(location = 0) out
};

cbuffer FMatrixUBO : register(b0)
{
    float4x4 Projection;
    float4x4 Viewing;
    float4x4 Model;

    float4x4 InverseProjection;
    float4x4 InverseViewing;
    float4x4 CofactorModel;
}MatrixUBO;

[shader("vertex")]
VertexOutput VertexMain(VertexInput _Input)
{
    float4 WorldPos   = mul(float4(_Input.Position, 1.0), MatrixUBO.Model);
    float4 ViewingPos = mul(WorldPos, MatrixUBO.Viewing);
    float4 ClippedPos = mul(ViewingPos, MatrixUBO.Projection);

    VertexOutput Output;
    Output.Position = ClippedPos;
    Output.Normal = mul(MatrixUBO.CofactorModel, float4(_Input.Normal,1.0)).xyz;
    return Output;
}

// ==================== Fragment Shader ====================

struct FragmentInput
{
    float3 Normal : TEXCOORD0; // Matches vertex output
    float4 Position : SV_Position;
};

[shader("fragment")]
float4 FragmentMain(FragmentInput _Input) : SV_TARGET
{
    // Bling-Phone
    float3 Light = { 1.0, 0, 0 };
    float3 LightClipPos = { 0.0, 2.0, 0.0 };
    float3 LightDir = normalize(LightClipPos - _Input.Position.xyz);

    float3 Albedo = { 0.5, 0.5, 0.0 };
    float3 AmbientColor = Albedo * Light;

    float3 DiffuseColor = max(dot(_Input.Normal, LightDir), 0.0);

    return float4(AmbientColor + DiffuseColor, 1.0);
    //return float4(_Input.Normal, 1.0); // Use interpolated normal as color
}