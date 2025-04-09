#include "Utility.h"

#include <cmath>
#include <random>

XMVECTOR Math::SphericalToCartesian(float radius, float theta, float phi)
{
    const float x = radius * std::sin(phi) * std::cos(theta);
    const float y = radius * std::cos(phi);
    const float z = radius * std::sin(phi) * std::sin(theta);
    return XMVectorSet(x, y, z, 1.0f);
}

float Math::AngleFromXY(float x, float y)
{
    float theta = std::atan2(y, x);
    theta = theta < 0.0f ? theta + XM_2PI : theta;
    return theta;
}

int Math::GetRandomInt(int min, int max)
{
    thread_local std::mt19937 generator(std::random_device{}());
    return std::uniform_int<>(min, max)(generator);
}

float Math::GetRandomFloat(float min, float max)
{
    thread_local std::mt19937 generator(std::random_device{}());
    return static_cast<float>(std::uniform_real<>(min, max)(generator));
}
