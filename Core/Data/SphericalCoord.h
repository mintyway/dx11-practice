#pragma once

#include "DirectXMath.h"
#include <cmath>

struct SphericalCoord
{
    // 구면 좌표(세타 가로 회전)-> 데카르트 좌표(Y-up 왼손 좌표계)
    [[nodiscard]]
    static DirectX::XMVECTOR SphericalToCartesian(const SphericalCoord& inSphericalCoord)
    {
        const float x = inSphericalCoord.radius * std::sin(inSphericalCoord.phi) * std::cos(inSphericalCoord.theta);
        const float y = inSphericalCoord.radius * std::cos(inSphericalCoord.phi);
        const float z = inSphericalCoord.radius * std::sin(inSphericalCoord.phi) * std::sin(inSphericalCoord.theta);
        return DirectX::XMVectorSet(x, y, z, 1.0f);
    }

    float radius;
    float theta;
    float phi;
};
