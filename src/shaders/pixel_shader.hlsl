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
    float padding;
    float3 color;
    float intensity;
};

cbuffer lightBuffer : register(b1)
{
    LightSource lightSources[16];
    uint numLights;
};

cbuffer modelColorRootConstant : register(b2)
{
    float3 modelColor;
};

[RootSignature(ROOTSIG)]
void main(in PS_Input input, out PS_Output output)
{
    output.color = float4(modelColor, 1.f);

    for (uint i = 0; i < numLights; i++)
    {
        float3 lightDirection = normalize(lightSources[i].position - input.worldPosition);
        float lightIntensity = max(0, dot(input.normal, lightDirection));

        output.color += float4(lightSources[i].color * lightIntensity, 0.0f) * lightSources[i].intensity;
    }

    output.color = float4(modelColor, 1.f);
}
