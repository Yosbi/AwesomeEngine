
/*cbuffer EngineVariables: register(b0)
{
    float4x4 gWVP;                          // World view proj combo matrix;
    float4x4 gWorldInverseTranspose;        // World matrix inversed and transposed
    
    float4 gDiffuseLight;                   // Diffuse light vector
    float4 gLightVecW;                      // Diffuse Light vector
    float4 gAmbientLight;                   // Ambient light
    float4 gSpecularLight;                  // Specular light vector
};*/


/*cbuffer cbMaterial : register(b1)
{
    float4 gDiffuseMtrl;                    // RGBA diffuse light
    float4 gAmbientMtrl;                    // RGBA ambient light
    float4 gSpecularMtrl;                   // RGBA specular light
    float4 gEmissiveMtrl;                   // RGBA emissive light
    float gSpecularPowerMtrl;               // Specular power
};*/

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;                    // World view proj combo matrix;
    float4x4 gWorldInverseTranspose;    // World matrix inversed and transposed
    
    float4 gDiffuseMtrl; // RGBA diffuse light
    float4 gAmbientMtrl; // RGBA ambient light
    float4 gSpecularMtrl; // RGBA specular light
    float4 gEmissiveMtrl; // RGBA emissive light
    float gSpecularPowerMtrl; // Specular power
}

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float4   gEyePosW;
    
    float4 gDiffuseLight; // Diffuse light vector
    float4 gLightVecW; // Diffuse Light vector
    float4 gAmbientLight; // Ambient light
    float4 gSpecularLight; // Specular light vector
}

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
    float3 normalW = mul(float4(IN.Normal, 0.0f), gWorldInverseTranspose).xyz;
    normalW = normalize(normalW);
    
    // Compute the color.
    // Compute diffuse lightning
    float s = max(dot(gLightVecW.xyz, normalW), 0.0f);
    float3 diffuse = s * (gDiffuseMtrl * gDiffuseLight).rgb;    
    
    // Compute ambient lightning
    float3 ambient = gAmbientMtrl.rgb * gAmbientLight.rgb;
    
    // Computing the final color
    OUT.Color.rgb = ambient + diffuse;
    OUT.Color.a = gDiffuseMtrl.a;

    // Transform position to homogeneus clip space
    float4 posW = mul(float4(IN.Position, 1.0f), gWorld);
    OUT.Position = mul(posW, gViewProj);
    //OUT.Color = IN.Color;

    return OUT;
}