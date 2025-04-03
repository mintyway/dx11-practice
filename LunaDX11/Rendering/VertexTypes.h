#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <array>

using namespace DirectX;

struct Vertex
{
    friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex)
    {
        os << vertex.pos.x << ' ' << vertex.pos.y << ' ' << vertex.pos.z;
        return os;
    }
    
    XMFLOAT3 pos;
    XMFLOAT4 color;
};

constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> vertexDesc =
{
    D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    D3D11_INPUT_ELEMENT_DESC{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
