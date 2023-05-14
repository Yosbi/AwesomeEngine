#include "DefaultHeaderInput.hlsli"


VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;


    // Transform normal to world space.
    float3 normalW = mul(float4(IN.Normal, 0.0f), gWorldInverseTranspose).xyz;
    normalW = normalize(normalW);
    
    // Transform position to world
    float4 posW = mul(float4(IN.Position, 1.0f), gWorld);
    
    //=======================================================
    // Compute the color.
    // Compute the vector from the vertex to the eye position.
    float3 toEye = normalize(gEyePosW - posW);

    // Compute the reflection vector.
    float3 r = reflect(-gLightVecW.xyz, normalW);

    // Determine how much (if any) specular light makes it
    // into the eye.
    float t = pow(max(dot(r, toEye), 0.0f), gSpecularPowerMtrl);

    // Determine the diffuse light intensity that strikes the vertex.
    float s = max(dot(gLightVecW.xyz, normalW), 0.0f);

    // Compute the ambient, diffuse, and specular terms separately.
    float3 spec = t * (gSpecularMtrl * gSpecularLight).rgb;
    float3 diffuse = s * (gDiffuseMtrl * gDiffuseLight).rgb;
    float3 ambient = gAmbientMtrl * gAmbientLight;
    
    // Sum all the terms together and copy over the diffuse alpha.
    OUT.Color.rgb = ambient + diffuse + spec;
    OUT.Color.a = gDiffuseMtrl.a;

    
    // Compute diffuse lightning
    /*float s = max(dot(gLightVecW.xyz, normalW), 0.0f);
    float3 diffuse = s * (gDiffuseMtrl * gDiffuseLight).rgb;    
    
    // Compute ambient lightning
    float3 ambient = gAmbientMtrl.rgb * gAmbientLight.rgb;
    
    // Computing the final color
    OUT.Color.rgb = ambient + diffuse;
    OUT.Color.a = gDiffuseMtrl.a;*/
     //=======================================================

    //  Transform position to homogeneus clip space
    OUT.Position = mul(posW, gViewProj);
   

    return OUT;
}