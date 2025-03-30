#pragma once
#include <DirectXMath.h>

using namespace DirectX;

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
