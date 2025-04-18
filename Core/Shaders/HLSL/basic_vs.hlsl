#include "SharedTypes.hlsli"

VertexOut VS_Main(VertexIn input)
{
    VertexOut output;
    output.positionWS = mul(float4(input.positionOS, 1.0f), worldMatrix).xyz;
    output.normalWS = mul(input.normalOS, (float3x3)worldInverseTransposeMatrix);
    output.positionCS = mul(float4(input.positionOS, 1.0f), wvpMatrix);
    return output;
}
