#include "root_signature.hlsli"
#include "structs.hlsli"

cbuffer cameraCBuffer : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

[RootSignature(ROOTSIG)]
void main(in VS_Input input, out VS_Output output)
{
    float4 worldPosition = float4(input.position, 1.0f);

    output.position = mul(projectionMatrix, worldPosition);
    output.worldPosition = input.position;
}
