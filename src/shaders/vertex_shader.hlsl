#include "root_signature.hlsli"

float4x4 projectionMatrix : register(b0);

[RootSignature(ROOTSIG)]
float4 main(float3 position : Position) : SV_Position
{
    float4 worldPos = float4(position, 1.0f);
    return mul(projectionMatrix, worldPos);
}
