#ifndef STRUCTS_HLSLI
#define STRUCTS_HLSLI

struct VS_Input
{
    float3 position : POSITION;
};

struct VS_Output
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
};

struct GS_Input
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
};

struct GS_Output
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normal : NORMAL;
};

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

#endif // STRUCTS_HLSLI
