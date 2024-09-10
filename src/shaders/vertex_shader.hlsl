#include "root_signature.hlsli"

struct VS_Input
{
    float3 position : POSITION;
};

struct VS_Output
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
};

cbuffer cameraCBuffer : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

[RootSignature(ROOTSIG)]
void main(in VS_Input input, out VS_Output output)
{
    float4 worldPosition = mul(viewMatrix, float4(input.position, 1.0f));

    output.position = mul(projectionMatrix, worldPosition);
    output.worldPosition = input.position;
}
