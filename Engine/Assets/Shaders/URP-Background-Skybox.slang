const static float2 Vertices[3] =
{
        { -1.0,  3.0 },
        { -1.0, -1.0 },
        {  3.0, -1.0 },
};

[shader("vertex")]
float4 VertexMain(uint _VertexID : SV_VertexID) : SV_Position
{
    return float4(Vertices[_VertexID], 0.0, 1.0);
}

//<< Fragment Shader>>
[shader("fragment")]
float4 FragmentMain() : SV_Target
{
    return float4(1.0, 1.0, 1.0, 1.0);
}