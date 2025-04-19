#pragma once

#include <DirectXMath.h>

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
