#pragma once

#include <cstdint>

#include <DirectXMath.h>

using namespace DirectX;

struct Color
{
    static Color LinearColorToColor(FXMVECTOR inLinearColor)
    {
        const XMVECTOR clamped = XMVectorClamp(inLinearColor, XMVectorZero(), XMVectorReplicate(1.0f));
        const XMVECTOR scaled =  clamped * 255.0f;
        XMUINT4 color;
        XMStoreUInt4(&color, XMVectorRound(scaled));
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
