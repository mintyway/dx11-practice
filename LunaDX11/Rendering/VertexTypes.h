#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <array>

using namespace DirectX;

struct SimpleVertex
{
    XMFLOAT3 pos;
    XMFLOAT3 color;
};

constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> SimpleVertexDesc =
{
    D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    D3D11_INPUT_ELEMENT_DESC{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

struct Vertex
{
    Vertex() = default;

    Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
        : position(p), normal(n), tangentU(t), texC(uv) {}

    Vertex(
        float px, float py, float pz,
        float nx, float ny, float nz,
        float tx, float ty, float tz,
        float u, float v)
        : position(px, py, pz), normal(nx, ny, nz),
          tangentU(tx, ty, tz), texC(u, v) {}

    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT3 tangentU;
    XMFLOAT2 texC;
};
