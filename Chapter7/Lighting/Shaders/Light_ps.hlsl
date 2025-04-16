#include "Core/Shaders/Light/LightingCommon.hlsli"
#include "Core/Shaders/Light/LightingFunction.hlsli"

cbuffer ConstantBufferPerFrame : register(b0)
{
    DirectionalLight directionaloLight;
    PointLight pointLight;
    SpotLight spotLight;
    float3 eyeWorldPosition;
}

void PS_Main()
{
    
}
