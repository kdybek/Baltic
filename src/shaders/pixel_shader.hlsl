#include "root_signature.hlsli"

struct PS_Input
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
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

cbuffer lightBuffer : register(b0)
{
    LightSource lightSources[15];
    uint numLights;
};

cbuffer modelColorRootConst : register(b1)
{
    float3 modelColor;
};

cbuffer absTimeRootConst : register(b2)
{
    float absTimeMod2Pi;
}

[RootSignature(ROOTSIG)]
void main(in PS_Input input, out PS_Output output)
{
    output.color = float4(0.f, 0.f, 0.f, 1.f);
    float3 normal = normalize(float3(-cos(absTimeMod2Pi + input.worldPosition.x), 1.f, 0.f));

    for (uint i = 0; i < numLights; i++)
    {
        float3 lightDirection = normalize(lightSources[i].position - input.worldPosition);
        float lightIntensity = max(0, dot(normal, lightDirection)) / pow(distance(lightSources[i].position, input.worldPosition), 2);

        output.color += float4(lightSources[i].color * lightIntensity, 0.f) * lightSources[i].intensity;
    }

    output.color = saturate(output.color) * float4(modelColor, 1.f);
}
