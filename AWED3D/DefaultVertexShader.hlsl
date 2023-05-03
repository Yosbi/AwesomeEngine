
cbuffer EngineVariables: register(b0)
{
    float4x4 gWVP;                          // World view proj combo matrix;
    float4 gDiffuseLight;                   // Duffuse light vector
    float4 gSpecularLight;                  // Specular light vector
    float4 gLightVecW;                      // Global Light vector
};


cbuffer cbMaterial : register(b1)
{
    float4 gDiffuseMtrl;                    // RGBA diffuse light
    float4 gAmbientMtrl;                    // RGBA ambient light
    float4 gSpecularMtrl;                   // RGBA specular light
    float4 gEmissiveMtrl;                   // RGBA emissive light
    float gSpecularPowerMtrl;               // Specular power
};

struct VertexPosColor
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 Texture : TEXTURE;
};

struct VertexShaderOutput
{
    float4 Color    : COLOR;
    float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    // Transform normal to world space.
    float3 normalW = mul(float4(IN.Normal, 0.0f), gWVP).xyz;
    normalW = normalize(normalW);
    
    //SV_DebugPrint(1, "gDiffuseMtrl  = (%f, %f, %f, %f)\n", gDiffuseMtrl.x, gDiffuseMtrl.y, gDiffuseMtrl.z, gDiffuseMtrl.w);

    // Compute the color.
    float s = max(dot(gLightVecW.xyz, normalW), 0.0f);
    OUT.Color.rgb = s * (gDiffuseMtrl * gDiffuseLight).rgb;;
    OUT.Color.a = gDiffuseMtrl.a;
    //OUT.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

    OUT.Position = mul(float4(IN.Position, 1.0f), gWVP);
    //OUT.Color = IN.Color;

    return OUT;
}