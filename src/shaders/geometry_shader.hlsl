#include "root_signature.hlsli"

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

[RootSignature(ROOTSIG)]
[maxvertexcount(3)]
void main(triangle GS_Input input[3], inout TriangleStream<GS_Output> triStream)
{
    float3 edge1 = input[1].worldPosition - input[0].worldPosition;
    float3 edge2 = input[2].worldPosition - input[0].worldPosition;

    float3 normal = normalize(cross(edge1, edge2));

    for (int i = 0; i < 3; i++)
    {
        GS_Output output;
        output.position = input[i].position;
        output.worldPosition = input[i].worldPosition;
        output.normal = normal;
        
        triStream.Append(output);
    }

    triStream.RestartStrip();
}
