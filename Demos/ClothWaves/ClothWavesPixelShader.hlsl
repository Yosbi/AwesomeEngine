#include "ClothWavesHeader.hlsli"

float4 main(VertexShaderOutput IN) : SV_Target
{
    float3 normalW;
    float dhOverdx;
    float dhOverdz;
    float3 u;
    float3 v;

	// Get the partials dx and dz of the point to calculate the normal
    Partials(IN.PositionW.x, IN.PositionW.z, dhOverdx, dhOverdz);
	
	//calculate the normal
    if (((IN.PositionW.x * IN.PositionW.x) < 0.01f) && ((IN.PositionW.z * IN.PositionW.z) < 0.01f))
    {
        normalW = float3(0.0f, 1.0f, 0.0f);
    }
    else
    {
        v = float3(1.0f, dhOverdx, 0.0f);
        u = float3(0.0f, dhOverdz, 1.0f);

        normalW = normalize(cross(u, v));
    }

	//Compute the color
	//=====================================================
	// Specular color
	// compute the vector from the vertex to the eye position
    float3 toEye = normalize(gEyePosW - IN.PositionW);
    
    // Compute halfway vector
    //float3 h = normalize(gLightVecW.xyz + toEye);
    float3 r = reflect(-gLights[0].vcDirectionW.xyz, normalW);
    
    // Determine how much(if any) specular light makes into the eye
    float t = pow(max(dot(r, toEye), 0.0f), gSpecularPowerMtrl);
    
    float3 spec = t * (gSpecularMtrl * gLights[0].cSpecularLight).rgb;
    
    // Diffuse color
    float s = max(dot(gLights[0].vcDirectionW.xyz, normalW), 0.0f);
    float3 diffuse = s * (gDiffuseMtrl * gLights[0].cDiffuseLight).rgb;
    
    // Them ambient color
    float3 ambient = gAmbientMtrl * gAmbientLight;

	// Finally the final color
    return float4((ambient + diffuse + spec), gDiffuseMtrl.a);
	
	//====================================================
}