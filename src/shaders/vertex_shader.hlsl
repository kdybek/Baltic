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

cbuffer constantBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

cbuffer absTimeRootConst : register(b1)
{
    float absTimeMod2Pi;
}

[RootSignature(ROOTSIG)]
void main(in VS_Input input, out VS_Output output)
{
    float4 absolutePosition = mul(worldMatrix, float4(input.position, 1.0f));
    absolutePosition.y += sin(absTimeMod2Pi + absolutePosition.x);
    float4 relativePosition = mul(viewMatrix, absolutePosition);

    output.position = mul(projectionMatrix, relativePosition);
    output.worldPosition = absolutePosition.xyz;
}
