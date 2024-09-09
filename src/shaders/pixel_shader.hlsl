#include "root_signature.hlsli"
#include "structs.hlsli"

[RootSignature(ROOTSIG)]
void main(in PS_Input input, out PS_Output output)
{
    float3 lightSource = float3(0.0f, 0.0f, 0.9f);
    float3 lightDirection = normalize(lightSource - input.worldPosition);
    float lightIntensity = saturate(dot(input.normal, lightDirection));

    output.color = float4(lightIntensity, lightIntensity, lightIntensity, 1.0f);
}
