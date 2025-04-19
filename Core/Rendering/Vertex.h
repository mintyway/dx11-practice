#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>

namespace Vertex
{
    struct PN
    {
        static inline const std::vector<D3D11_INPUT_ELEMENT_DESC> Desc =
        {
            D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
            D3D11_INPUT_ELEMENT_DESC{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA}
        };

        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
    };
}
