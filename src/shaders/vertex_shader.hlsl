#include "root_signature.hlsli"

[RootSignature(ROOTSIG)]
float4 main(float2 position : Position) : SV_Position
{
    return float4(position, 0.0f, 1.0f);
}
