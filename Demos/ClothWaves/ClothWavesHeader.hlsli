#define MaxLights 16

#define NotSetLight 0
#define DirectionalLight 1
#define PointLight 2
#define SpotLight 3

struct Light
{
    float4 cDiffuseLight; // Diffuse light color
    float4 cSpecularLight; // Specular light color

    float4 vcPositionW; // Diffuse light vector
    float4 vcDirectionW; // Light direction

    float fRange; // Range of light
    float fTheta; // Angle of spot light inner cone
    float fPhi; // Angle of spot light outer cone
    float fAttenuation0; // Change of intensity over distance
    float fAttenuation1; // Change of intensity over distance
    float fAttenuation2; // Change of intensity over distance
    
    int Type; // Type of light
    float notUsed;
};

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
    float4 gAmbientLight; // Ambient light
    Light gLights[MaxLights];
    float gTime; // Delta time
}

struct VertexPosNormalTex
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Texture : TEXTURE;
};

struct VertexShaderOutput
{
    float4 PositionW : POSITION;
    float3 NormalH : NORMAL;
    float4 PositionH : SV_Position;
};

// Specific code for this shader

// Amplitudes
static float a[3] = { 0.8f, 0.2f, 0.08f };
	
// Angular wave numbers.
static float k[3] = { 1.0, 0.8f, 2.0f };
	
// Angular frequency.
static float w[3] = { 1.0f, 8.0f, 10.0f };
	
// Phase shifts.
static float p[3] = { 0.0f, 1.0f, 1.0f };

float SumOfRadialSineWaves(float x, float z)
{
	// Distance of vertex from source of waves (which we set
	// as the origin of the local space).
    float d = sqrt(x * x + z * z);
	//float d = sqrt (z);
	
	// Sum the waves.
    float sum = 0.0f;
    for (int i = 0; i < 2; ++i)
        sum += a[i] * sin(k[i] * d - gTime * w[i] + p[i]);
		
    return sum;
}

void Partials(float x, float z, out float dhOverdx, out float dhOverdz)
{
    // Distance of vertex from source of waves (which we set
    // as the origin of the local space).
    float d = sqrt(x * x + z * z);

    // Derivative of a sum of functions is the sum of the derivatives.
    dhOverdx = 0.0f;
    dhOverdz = 0.0f;
    for (int i = 0; i < 2; ++i)
    {
        dhOverdx += (a[i] * k[i] * x * cos(k[i] * d - gTime * w[i] + p[i])) / d;
        dhOverdz += (a[i] * k[i] * z * cos(k[i] * d - gTime * w[i] + p[i])) / d;
    }
}
