#pragma once

#include "Core/Light/Light.h"
#include <DirectXMath.h>

using namespace DirectX;

struct ConstantBufferPerObjecteData
{
    XMFLOAT4X4 worldMatrix;
    XMFLOAT4X4 worldInverseTransposeMatrix;
    XMFLOAT4X4 wvpMatrix;
    Material material;
};

struct ConstantBufferPerFrameData
{
    DirectionalLight directionalLight;
    PointLight pointLight;
    SpotLight spotLight;
    alignas(16) XMFLOAT3 eyeWorldPosition;
};
