#pragma once

#include <DirectXMath.h>

#define CHECK_HR(hr, text, ...)\
if (FAILED(hr))\
{\
MessageBox(nullptr, text, L"Error", MB_OK | MB_ICONERROR);\
return __VA_ARGS__;\
}

namespace Colors
{
    constexpr DirectX::XMVECTORF32 White = {1.0f, 1.0f, 1.0f, 1.0f};
    constexpr DirectX::XMVECTORF32 Black = {0.0f, 0.0f, 0.0f, 1.0f};
    constexpr DirectX::XMVECTORF32 Red = {1.0f, 0.0f, 0.0f, 1.0f};
    constexpr DirectX::XMVECTORF32 Green = {0.0f, 1.0f, 0.0f, 1.0f};
    constexpr DirectX::XMVECTORF32 Blue = {0.0f, 0.0f, 1.0f, 1.0f};
    constexpr DirectX::XMVECTORF32 Yellow = {1.0f, 1.0f, 0.0f, 1.0f};
    constexpr DirectX::XMVECTORF32 Cyan = {0.0f, 1.0f, 1.0f, 1.0f};
    constexpr DirectX::XMVECTORF32 Magenta = {1.0f, 0.0f, 1.0f, 1.0f};
    constexpr DirectX::XMVECTORF32 Silver = {0.75f, 0.75f, 0.75f, 1.0f};
    constexpr DirectX::XMVECTORF32 LightSteelBlue = {0.69f, 0.77f, 0.87f, 1.0f};
}

namespace Math
{
    [[nodiscard]]
    float AngleFromXY(float x, float y);

    [[nodiscard]]
    int GetRandomInt(int min, int max);

    [[nodiscard]]
    float GetRandomFloat(float min, float max);
}
