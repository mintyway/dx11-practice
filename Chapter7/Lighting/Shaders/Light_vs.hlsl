#include "Core/Shaders/Light/LightingCommon.hlsli"

cbuffer ConstantBufferPerObject
{
    float4x4 worldMatrix;
    float4x4 worldInverseTransposeMatrix;
    float4x4 wvpMatrix;
    Material material;
};

void VS_Main()
{
    
}