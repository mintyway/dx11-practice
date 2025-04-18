#ifndef LIGHTING_SHARED_TYPES_HLSLI
#define LIGHTING_SHARED_TYPES_HLSLI

#include "Core/Shaders/HLSL/LightingCommon.hlsli"
#include "Core/Shaders/HLSL/LightingFunction.hlsli"

cbuffer ObjectRenderCBuffer : register(b0)
{
    row_major float4x4 worldMatrix;
    row_major float4x4 worldInverseTransposeMatrix;
    row_major float4x4 wvpMatrix;
    Material material;
};

cbuffer SceneLightCBuffer : register(b1)
{
    DirectionalLight directionalLight;
    PointLight pointLight;
    SpotLight spotLight;
    float3 eyeWorldPosition;
}

struct VertexIn
{
    float3 positionOS : POSITION;
    float3 normalOS : NORMAL;
};

struct VertexOut
{
    float3 positionWS : POSITION;
    float4 positionCS : SV_Position;
    float3 normalWS : NORMAL;
};

#endif // LIGHTING_SHARED_TYPES_HLSLI
