#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight
{
    XMFLOAT4 ambient;
    XMFLOAT4 diffuse;
    XMFLOAT4 specular;

    alignas(16) XMFLOAT3 direction;
};

struct PointLight
{
    XMFLOAT4 ambient;
    XMFLOAT4 diffuse;
    XMFLOAT4 specular;

    XMFLOAT3 position;
    float range;

    alignas(16) XMFLOAT3 attenuation;
};

struct SpotLight
{
    XMFLOAT4 ambient;
    XMFLOAT4 diffuse;
    XMFLOAT4 specular;

    XMFLOAT3 position;
    float range;

    XMFLOAT3 direction;
    float spot;

    alignas(16) XMFLOAT3 attenuation;
};

struct Material
{
    XMFLOAT4 ambient;
    XMFLOAT4 diffuse;
    XMFLOAT4 specular;
    XMFLOAT4 reflect;
};
