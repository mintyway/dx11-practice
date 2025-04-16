#pragma once

#include "Core/Utilities/Color.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <array>

using namespace DirectX;

struct Vertex
{
    static constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> Desc =
    {
        D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
        D3D11_INPUT_ELEMENT_DESC{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA}
    };

    XMFLOAT3 position;
    XMFLOAT3 normal;
};

struct VertexWithColor
{
    static constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> Desc =
    {
        D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
        D3D11_INPUT_ELEMENT_DESC{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA}
    };

    XMFLOAT3 position;
    Color color;
};

struct VertexWithLinearColor
{
    static constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> Desc =
    {
        D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        D3D11_INPUT_ELEMENT_DESC{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    friend std::ostream& operator<<(std::ostream& os, const VertexWithLinearColor& vertex)
    {
        os << vertex.position.x << ' ' << vertex.position.y << ' ' << vertex.position.z;
        return os;
    }

    XMFLOAT3 position;
    XMFLOAT4 linearColor;
};
