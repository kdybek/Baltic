#include "root_signature.hlsli"

float4x4 projectionMatrix : register(b0);

struct VS_Input
{
    float3 position : POSITION;
};

struct VS_Output
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
};

[RootSignature(ROOTSIG)]
void main(in VS_Input input, out VS_Output output)
{
    float4 worldPosition = float4(input.position, 1.0f);

    output.position = mul(projectionMatrix, worldPosition);
    output.worldPosition = input.position;
}
