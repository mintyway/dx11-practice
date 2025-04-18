#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <array>

using namespace DirectX;

namespace Vertex
{
    struct PN
    {
        static constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> Desc =
        {
            D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
            D3D11_INPUT_ELEMENT_DESC{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA}
        };

        XMFLOAT3 position;
        XMFLOAT3 normal;
    };
}
