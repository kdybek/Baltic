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

[RootSignature(ROOTSIG)]
void main(in PS_Input input, out PS_Output output)
{
    float3 lightSource = float3(0.0f, 0.0f, 0.9f);
    float3 lightDirection = normalize(lightSource - input.worldPosition);
    float lightIntensity = saturate(dot(input.normal, lightDirection));

    output.color = float4(lightIntensity, lightIntensity, lightIntensity, 1.0f);
}
