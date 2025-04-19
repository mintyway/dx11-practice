#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

class GeometryGenerator
{
public:
    struct Vertex
    {
        Vertex() = default;

        Vertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT3& t, const DirectX::XMFLOAT2& uv) : position(p), normal(n), tangentU(t), texC(uv) {}

        Vertex(float px, float py, float pz,
               float nx, float ny, float nz,
               float tx, float ty, float tz,
               float u, float v)
            : position(px, py, pz), normal(nx, ny, nz),
              tangentU(tx, ty, tz), texC(u, v) {}

        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT3 tangentU;
        DirectX::XMFLOAT2 texC;
    };

    struct MeshData
    {
        std::vector<Vertex> vertices;
        std::vector<UINT> indices;
    };

    [[nodiscard]]
    static MeshData CreateBox(float width, float height, float depth);

    [[nodiscard]]
    static MeshData CreateSphere(float radius, UINT sliceCount, UINT stackCount);

    [[nodiscard]]
    static MeshData CreateGeodesicSphere(float radius, UINT subdivisionCount);

    [[nodiscard]]
    static MeshData CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount);

    [[nodiscard]]
    static MeshData CreateGrid(float width, float depth, UINT rowVertexCount, UINT columnVertexCount);

private:
    static void Subdivide(MeshData& meshData);
};
