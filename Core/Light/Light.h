#pragma once

#include <DirectXMath.h>

struct DirectionalLight
{
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 diffuse;
    DirectX::XMFLOAT4 specular;

    alignas(16) DirectX::XMFLOAT3 direction;
};

struct PointLight
{
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 diffuse;
    DirectX::XMFLOAT4 specular;

    DirectX::XMFLOAT3 position;
    float range;

    alignas(16) DirectX::XMFLOAT3 attenuation;
};

struct SpotLight
{
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 diffuse;
    DirectX::XMFLOAT4 specular;

    DirectX::XMFLOAT3 position;
    float range;

    DirectX::XMFLOAT3 direction;
    float spot;

    alignas(16) DirectX::XMFLOAT3 attenuation;
};

struct Material
{
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 diffuse;
    DirectX::XMFLOAT4 specular;
    DirectX::XMFLOAT4 reflect;
};
