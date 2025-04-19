#pragma once

#include <DirectXMath.h>

namespace LinearColors
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

struct Color
{
    static Color LinearColorToColor(DirectX::FXMVECTOR inLinearColor)
    {
        const DirectX::XMVECTOR clamped = DirectX::XMVectorClamp(inLinearColor, DirectX::XMVectorZero(), DirectX::XMVectorReplicate(1.0f));
        const DirectX::XMVECTOR scaled = DirectX::XMVectorScale(clamped, 255.0f);
        DirectX::XMUINT4 color;
        XMStoreUInt4(&color, DirectX::XMVectorRound(scaled));
        return {static_cast<uint8_t>(color.x), static_cast<uint8_t>(color.y), static_cast<uint8_t>(color.z), static_cast<uint8_t>(color.w)};
    }

    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) : component{r, g, b, a} {}

    struct Component
    {
        uint8_t r, g, b, a;
    };

    union
    {
        uint32_t rgba;
        Component component;
    };
};
