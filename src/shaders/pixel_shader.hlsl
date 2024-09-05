#include "root_signature.hlsli"

[RootSignature(ROOTSIG)]
float4 main(float4 color : Color) : SV_Target
{
    return color;
}
