const static float2 Vertices[3] =
{
        { -1.0,  3.0 },
        { -1.0, -1.0 },
        {  3.0, -1.0 },
};

const static float2 UVCoords[3] =
{
        { -1.0, 3.0 },
        { -1.0, -1.0 },
        { 3.0, -1.0 },
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
    VSOut.UVCoord = (Vertices[_VertexID] + float2(1.0, 1.0)) * 0.25;
    return VSOut;
}

//<< Fragment Shader>>
Texture2D<float4>   SVColorTexture : register(t0);
SamplerState        SVColorSampler : register(s0);

[shader("fragment")]
float4 FragmentMain(float2 UVs : TEXCOORD0) : SV_Target
{
    return SVColorTexture.Sample(SVColorSampler, UVs);
}