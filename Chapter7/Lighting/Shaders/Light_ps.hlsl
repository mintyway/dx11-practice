#include "Core/Shaders/Light/LightingCommon.hlsli"

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
