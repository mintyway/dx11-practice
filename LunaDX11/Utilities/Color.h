#pragma once

#include <cstdint>

struct Color
{
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
