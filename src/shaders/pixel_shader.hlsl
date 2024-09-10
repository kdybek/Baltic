#include "root_signature.hlsli"

struct PS_Input
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PS_Output
{
    float4 color : SV_TARGET;
};

struct LightSource
{
    float3 position;
    float3 color;
    float intensity;
};

cbuffer ConstantBuffer : register(b1)
{
    LightSource lightSources[16];
    uint numLights;
};

[RootSignature(ROOTSIG)]
void main(in PS_Input input, out PS_Output output)
{
    output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);

    for (uint i = 0; i < numLights; i++)
    {
        float3 lightDirection = normalize(lightSources[i].position - input.worldPosition);
        float lightIntensity = dot(input.normal, lightDirection);

        output.color += float4(lightSources[i].color * lightIntensity, 0.0f) * lightSources[i].intensity;
    }

    output.color = saturate(output.color);
}
