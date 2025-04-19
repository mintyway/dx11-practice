#pragma once

#include <DirectXMath.h>

#define CHECK_HR(hr, text, ...)\
if (FAILED(hr))\
{\
MessageBox(nullptr, text, L"Error", MB_OK | MB_ICONERROR);\
return __VA_ARGS__;\
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
