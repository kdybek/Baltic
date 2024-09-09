struct GS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
};

struct GS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normal : NORMAL;
};

[maxvertexcount(3)]
void main(triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> triStream)
{
    float3 edge1 = input[1].worldPosition - input[0].worldPosition;
    float3 edge2 = input[2].worldPosition - input[0].worldPosition;

    float3 faceNormal = normalize(cross(edge1, edge2));

    for (int i = 0; i < 3; i++)
    {
        GS_OUTPUT output;
        output.position = input[i].position;
        output.worldPosition = input[i].worldPosition;
        output.normal = faceNormal;
        
        triStream.Append(output);
    }

    triStream.RestartStrip();
}
