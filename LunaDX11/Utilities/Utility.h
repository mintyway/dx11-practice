#pragma once

#include <DirectXMath.h>

using namespace DirectX;

#define CHECK_HR(hr, text, ...)\
if (FAILED(hr))\
{\
MessageBox(nullptr, text, L"Error", MB_OK | MB_ICONERROR);\
return __VA_ARGS__;\
}

namespace Colors
{
    constexpr XMVECTORF32 White = {1.0f, 1.0f, 1.0f, 1.0f};
    constexpr XMVECTORF32 Black = {0.0f, 0.0f, 0.0f, 1.0f};
    constexpr XMVECTORF32 Red = {1.0f, 0.0f, 0.0f, 1.0f};
    constexpr XMVECTORF32 Green = {0.0f, 1.0f, 0.0f, 1.0f};
    constexpr XMVECTORF32 Blue = {0.0f, 0.0f, 1.0f, 1.0f};
    constexpr XMVECTORF32 Yellow = {1.0f, 1.0f, 0.0f, 1.0f};
    constexpr XMVECTORF32 Cyan = {0.0f, 1.0f, 1.0f, 1.0f};
    constexpr XMVECTORF32 Magenta = {1.0f, 0.0f, 1.0f, 1.0f};
    constexpr XMVECTORF32 Silver = {0.75f, 0.75f, 0.75f, 1.0f};
    constexpr XMVECTORF32 LightSteelBlue = {0.69f, 0.77f, 0.87f, 1.0f};
}

namespace Math
{
    // 구면 좌표(세타 가로 회전)-> 데카르트 좌표(Y-up 왼손 좌표계)
    [[nodiscard]]
    XMVECTOR SphericalToCartesian(float radius, float theta, float phi);

    [[nodiscard]]
    float AngleFromXY(float x, float y);

    [[nodiscard]]
    int GetRandomInt(int min, int max);

    [[nodiscard]]
    float GetRandomFloat(float min, float max);
}
