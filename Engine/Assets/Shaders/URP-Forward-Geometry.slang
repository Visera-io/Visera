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

    float4 FragPosLightSpace;
};

[[vk::binding(0, 0)]]
cbuffer FMatrixUBO : register(space0, b0)
{
    float4x4 Projection;
    float4x4 Viewing;
    float4x4 Model;

    float4x4 InverseProjection;
    float4x4 InverseViewing;
    float4x4 CofactorModel;
} MatrixUBO;

[[vk::binding(0, 1)]]
cbuffer FLightUBO : register(space1, b0)
{
    float4x4 VPMatrix;
} LightUBO;

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

    Output.FragPosLightSpace = mul(WorldPos, LightUBO.VPMatrix);
    return Output;
}

// ==================== Fragment Shader ====================

struct FragmentInput
{
    float4 Position : SV_Position;
    float3 Normal   : NORMAL; // Matches vertex output
    float3 UVCoord  : TEXCOORD0;

    float4 FragPosLightSpace;
};

[[vk::binding(0, 2)]]
Texture2D<float4> DiffuseTexture : register(space2, t0); // Texture
[[vk::binding(0, 2)]]
SamplerState TextureSampler      : register(space2, s0); // Sampler
[[vk::binding(0, 3)]]
Texture2D<float4> ShadowMap      : register(space3, t1); // Texture
[[vk::binding(0, 3)]]
SamplerState ShadowMapSampler    : register(space3, s1); // Sampler

float ComputeShadow(float4 _FragPosLightSpace)
{
    // Percentage Closer Filtering (PCF)
    float2 ProjCoords = _FragPosLightSpace.xy / _FragPosLightSpace.w;
    ProjCoords = ProjCoords * 0.5 + 0.5;

    float CameraViewDepth = _FragPosLightSpace.z / _FragPosLightSpace.w;
    float Bias = 0.001f;
    float Shadow = 0.0;
    float Width, Height, Levels;
    ShadowMap.GetDimensions(0, Width, Height, Levels);
    float2 Offsets = (1.0 / float2(Width, Height));
    for (float dx = -1.5; dx <= 1.5; dx += 1.0)
    {
        for (float dy = -1.5; dy <= 1.5; dy += 1.0)
        {
            float2 SampleCoords  = ProjCoords + Offsets * float2(dx, dy);
            float LightViewDepth = ShadowMap.Sample(ShadowMapSampler, SampleCoords).r;
            Shadow += (CameraViewDepth + Bias) > LightViewDepth ? 0.0 : 1.0; // Reversed Z
        }
    }
    return Shadow / 16.0;
}

[shader("fragment")]
float4 FragmentMain(FragmentInput _Input) : SV_TARGET
{
    // return float4(_Input.Normal, 1.0); // Use interpolated normal as color
    float Shadow = ComputeShadow(_Input.FragPosLightSpace);
    float3 DiffuseColor = (1.0 - Shadow) 
                        * DiffuseTexture.Sample(TextureSampler, _Input.UVCoord.xy).xyz;
    return float4(DiffuseColor, 1.0);
    
    //return float4(_Input.Normal, 1.0); // Use interpolated normal as color
}