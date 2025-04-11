// ==================== Vertex Shader ====================

struct VertexInput
{
    float3 Position : POSITION;     // layout(location = 0)
    float3 Normal   : NORMAL;       // layout(location = 1)
    float3 UVCoord  : TEXCOORD0;    // layout(location = 2)
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal   : NORMAL;
    float3 UVCoord  : TEXCOORD0;
};

cbuffer FMatrixUBO : register(b0)
{
    float4x4 Projection;
    float4x4 Viewing;
    float4x4 Model;

    float4x4 InverseProjection;
    float4x4 InverseViewing;
    float4x4 CofactorModel;

    float4 Padding;
}MatrixUBO;

[shader("vertex")]
VertexOutput VertexMain(VertexInput _Input)
{
    float4 WorldPos   = mul(float4(_Input.Position, 1.0), MatrixUBO.Model);
    float4 ViewingPos = mul(WorldPos, MatrixUBO.Viewing);
    float4 ClipPos    = mul(ViewingPos, MatrixUBO.Projection);
    
    VertexOutput Output;
    Output.Position = ClipPos;
    Output.Normal   = mul(float3x3(MatrixUBO.CofactorModel), _Input.Normal);
    Output.UVCoord  = _Input.UVCoord;
    return Output;
}

// ==================== Fragment Shader ====================

struct FragmentInput
{
    float4 Position : SV_Position;
    float3 Normal   : NORMAL; // Matches vertex output
    float3 UVCoord  : TEXCOORD0;
};

[[vk::binding(0, 1)]]
Texture2D<float4> DiffuseTexture : register(t0); // Texture
[[vk::binding(0, 1)]]
SamplerState      TextureSampler : register(s0);      // Sampler

[shader("fragment")]
float4 FragmentMain(FragmentInput _Input) : SV_TARGET
{
    // Bling-Phone
    float3 Light = { 1.0, 1.0, 1.0 };
    float3 LightClipPos = { 1.0, 2.0, 0.0 };
    float3 LightDir = normalize(LightClipPos - _Input.Position.xyz);

    float3 Albedo = { 0.5, 0.5, 0.5 };
    float3 AmbientColor = Albedo * Light;
    //return float4(_Input.Normal, 1.0); // Use interpolated normal as color
    float4 DiffuseColor = DiffuseTexture.Sample(TextureSampler, _Input.UVCoord.xy);
    return DiffuseColor;
    
    //float3 DiffuseColor = max(dot(_Input.Normal, LightDir), 0.5);

    //return float4(AmbientColor + DiffuseColor, 1.0);
    //return float4(_Input.Normal, 1.0); // Use interpolated normal as color
}