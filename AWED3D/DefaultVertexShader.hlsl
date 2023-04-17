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

    float3 gDiffuseLight = float3(1.0f, 1.0f, 1.0f);
    float3 gLightVecW = normalize(float3(0.0f, 1.0f, -1.0f));

    // Transform normal to world space.
    float3 normalW = mul(float4(IN.Normal, 0.0f), MVP).xyz;
    normalW = normalize(normalW);

    // Compute the color.
    float s = max(dot(gLightVecW, normalW), 0.0f);
    OUT.Color.rgb = s * (IN.Color.rgb * gDiffuseLight).rgb;
    OUT.Color.a = IN.Color.a;

    OUT.Position = mul(float4(IN.Position, 1.0f), MVP);
    //OUT.Color = IN.Color;

    return OUT;
}