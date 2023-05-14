cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld; // World view proj combo matrix;
    float4x4 gWorldInverseTranspose; // World matrix inversed and transposed
    
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
    float4 gEyePosW;
    
    float4 gDiffuseLight; // Diffuse light vector
    float4 gLightVecW; // Diffuse Light vector
    float4 gAmbientLight; // Ambient light
    float4 gSpecularLight; // Specular light vector
}

struct VertexPosColor
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Texture : TEXTURE;
};

struct VertexShaderOutput
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
};