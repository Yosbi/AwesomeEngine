#include "ClothWavesHeader.hlsli"


VertexShaderOutput main(VertexPosNormalTex IN)
{
    VertexShaderOutput OUT;

    // Transform normal to world space.
    float3 normalW = mul(float4(IN.Normal, 0.0f), gWorldInverseTranspose).xyz;
    OUT.NormalH = normalize(normalW);
    
    // Transform position to world
    OUT.PositionW = mul(float4(IN.Position, 1.0f), gWorld);
    
    //// find the Y coord
    OUT.PositionW.y = SumOfRadialSineWaves(OUT.PositionW.x, OUT.PositionW.z);
    
    //  Transform position to homogeneus clip space
    OUT.PositionH = mul(OUT.PositionW, gViewProj);

    return OUT;
}