const static float2 Vertices[3] =
{
    { -1.0,  3.0 },
    { -1.0, -1.0 },
    {  3.0, -1.0 },
};

const static float2 UVCoords[3] =
{
    { 0.0, 2.0 },
    { 0.0, 0.0 },
    { 2.0, 0.0 },
};

struct VertexOut
{
    float4 Position : SV_Position;
    float2 UVCoord  : TEXCOORD0;
}

[shader("vertex")]
VertexOut VertexMain(uint _VertexID: SV_VertexID)
{
    VertexOut VSOut;
    VSOut.Position = float4(Vertices[_VertexID], 0.0, 1.0);
    VSOut.UVCoord = UVCoords[_VertexID]; // to [0, 2]
    return VSOut;
}

//<< Fragment Shader>>
Texture2D<float4>   SVColorTexture : register(t0);
SamplerState        SVColorSampler : register(s0);

[shader("fragment")]
float4 FragmentMain(float2 UVs : TEXCOORD0) : SV_Target
{
    /*[Do NOT need Gamma Correction in URP]
     - We rendered in UNORM framebuffer, but the SV_Color is sampled in sRGB framebuffer,
       therefore, the GPU will automatically handle Gamma Correction.
     - If you want to confirm, try to do "return float4(pow(Color, Gamma), 1.0);"
    */
    return SVColorTexture.Sample(SVColorSampler, UVs);
    // float3 Color = SVColorTexture.Sample(SVColorSampler, UVs).rgb;
    // float3 Gamma = { 2.2, 2.2, 2.2 };

    // return float4(pow(Color, Gamma), 1.0);
}