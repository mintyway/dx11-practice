#include "Core/Shaders/Light/LightingCommon.hlsli"
#include "VertexType.hlsli"

cbuffer ConstantBufferPerObject : register(b0)
{
    row_major float4x4 worldMatrix;
    row_major float4x4 worldInverseTransposeMatrix;
    row_major float4x4 wvpMatrix;
    Material material;
};

VertexOut VS_Main(VertexIn input)
{
    VertexOut output;
    output.positionWS = mul(float4(input.positionOS, 1.0f), worldMatrix).xyz;
    output.normalWS = mul(input.normalOS, (float3x3)worldInverseTransposeMatrix);
    output.positionCS = mul(float4(input.positionOS, 1.0f), wvpMatrix);
    return output;
}
