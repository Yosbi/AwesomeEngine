cbuffer ModelViewProjection: register(b0)
{
    matrix MVP;
};

struct VertexPosColor
{
    float3 Position : POSITION;
    float4 Color    : COLOR;
    float2 Texture  : TEXTURE;
    float3 Normal   : NORMAL;
};

struct VertexShaderOutput
{
    float4 Color    : COLOR;
    float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(float4(IN.Position, 1.0f), MVP);
    OUT.Color = IN.Color;

    return OUT;
}