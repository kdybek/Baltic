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
    float3 sunlightDirection;
    float3 sunlightColor;
    float sunlightIntensity;
    float3 viewDirection;
};

cbuffer ModelBuffer : register(b1)
{
    float4x4 worldMatrix;
    float3 color;
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
    float specularPower;
    float padding;

};

cbuffer absTimeRootConst : register(b2)
{
    float absTimeMod2Pi;
};

[RootSignature(ROOTSIG)]
void main(in PS_Input input, out PS_Output output)
{
    float3 normal = normalize(float3(-cos(absTimeMod2Pi + input.worldPosition.x), 1.f, 0.f));

    float ambient = ambientIntensity;
    float diffuse = max(0, dot(normal, -sunlightDirection)) * diffuseIntensity;
    float specular = pow(max(0, dot(normal, normalize(-sunlightDirection + -viewDirection))), specularPower) * specularIntensity;

    output.color = float4(color * sunlightColor * sunlightIntensity * (ambient + diffuse + specular), 0.f);
}
