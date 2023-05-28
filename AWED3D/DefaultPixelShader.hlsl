#include "DefaultHeaderInput.hlsli"

float3 ComputeDirectionalLight(uint lightIndex, float3 normal, float3 toEye)
{
	// Compute the reflection vector
    float3 r = reflect(-gLights[lightIndex].vcDirectionW.xyz, normal);

	// Determine how much(if any) specular light makes into the eye
    float t = pow(max(dot(r, toEye), 0.0f), gSpecularPowerMtrl);
    float3 spec = t * (gSpecularMtrl * gLights[lightIndex].cSpecularLight).rgb;

	// Diffuse color
    float s = max(dot(gLights[lightIndex].vcDirectionW.xyz, normal), 0.0f);
    float3 diffuse = s * (gDiffuseMtrl * gLights[lightIndex].cDiffuseLight).rgb;

	// Finally the final color
    return diffuse + spec;
}

float3 ComputePointLight(uint i, float3 normal, float3 toEye, float3 posW)
{
	//Unit vector from vertex to light source.
    float3 lightVecW = normalize(gLights[i].vcPositionW.xyz - posW);
    
	// Diffuse Light Computation.
    float s = max(dot(normal, lightVecW), 0.0f);
    float3 diffuse = s * (gDiffuseMtrl * gLights[i].cDiffuseLight).rgb;
	
	// Specular Light Computation.
    float3 toEyeW = normalize(gEyePosW.xyz - posW);
    float3 reflectW = reflect(-lightVecW, normal);
    float t = pow(max(dot(reflectW, toEyeW), 0.0f), gSpecularPowerMtrl);
    float3 spec = t * (gSpecularMtrl * gLights[i].cSpecularLight).rgb;
    
    // Attentuation.
    float d = distance(gLights[i].vcPositionW.xyz, posW);
    float A = gLights[i].fAttenuation0 + gLights[i].fAttenuation1 * d + gLights[i].fAttenuation2 * d * d;
    
    return (diffuse + spec) / A;
}

float3 ComputeLighting(float3 normal, float3 toEye, float3 posW)
{
    float3 result = 0.0f;
    for (int i = 0; i < 16; ++i)
    {
        if (gLights[i].Type == DirectionalLight)
            result += ComputeDirectionalLight(i, normal, toEye);
        
        if (gLights[i].Type == PointLight)
            result += ComputePointLight(i, normal, toEye, posW);
        
        if (gLights[i].Type == SpotLight)
            break;
        
        if (gLights[i].Type == NotSetLight)
            break;

     }    
    return result;
}

float4 main(VertexShaderOutput IN) : SV_Target
{
	// Interpolated normals can become unnormal so normalize
    IN.NormalH = normalize(IN.NormalH);

	//Compute the color
	//=====================================================
	// Specular color
	// compute the vector from the vertex to the eye position
    float3 toEye = normalize(gEyePosW - IN.PositionW);
    
    float3 light =  ComputeLighting(IN.NormalH, toEye, IN.PositionW.xyz);
    
    // Them ambient color
    float3 ambient = gAmbientMtrl.rgb * gAmbientLight.rgb;
    
    return float4((ambient + light), gDiffuseMtrl.a);

	
	//====================================================
}