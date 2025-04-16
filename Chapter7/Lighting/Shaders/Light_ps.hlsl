#include "Core/Shaders/Light/LightingCommon.hlsli"
#include "Core/Shaders/Light/LightingFunction.hlsli"

cbuffer ConstantBufferPerFrame : register(b0)
{
    DirectionalLight directionaloLight;
    PointLight pointLight;
    SpotLight spotLight;
    float3 eyeWorldPosition;
}

struct VertexOut
{
    float3 positionWS : POSITION;
    float4 positionCS : SV_Position;
    float3 normalWS : NORMAL;
};

float4 PS_Main(VertexOut input) : SV_Target
{
    input.normalWS = normalize(input.normalWS);

    const float3 toEyeWS = normalize(eyeWorldPosition - input.positionWS);

    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 a, d, s;

    float4 litColor;
    return litColor;
}
