#include "GeometryGenerator.h"

#include <vector>

GeometryGenerator::MeshData GeometryGenerator::CreateGrid(float width, float depth, UINT rowVertexCount, UINT columnVertexCount)
{
    const UINT widthCellCount = columnVertexCount - 1;
    const UINT depthCellCount = rowVertexCount - 1;

    const UINT vertexCount = rowVertexCount * columnVertexCount;
    const UINT triangleCount = 2 * widthCellCount * depthCellCount;

    const float halfWidth = 0.5f * width;
    const float halfDepth = 0.5f * depth;

    const float dx = width / static_cast<float>(widthCellCount);
    const float dz = depth / static_cast<float>(depthCellCount);

    const float du = 1.0f / static_cast<float>(widthCellCount);
    const float dv = 1.0f / static_cast<float>(depthCellCount);

    MeshData meshData;
    meshData.vertices.resize(vertexCount);
    for (UINT i = 0; i < rowVertexCount; ++i)
    {
        const float z = halfDepth - static_cast<float>(i) * dz;
        for (UINT j = 0; j < columnVertexCount; ++j)
        {
            const float x = -halfWidth + static_cast<float>(j) * dx;

            Vertex& vertex = meshData.vertices[i * columnVertexCount + j];
            vertex.position = XMFLOAT3(x, 0.0f, z);
            vertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
            vertex.tangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);
            vertex.texC = XMFLOAT2(static_cast<float>(j) * du, static_cast<float>(i) * dv);
        }
    }

    meshData.indices.reserve(triangleCount * 3);
    for (UINT i = 0; i < rowVertexCount - 1; ++i)
    {
        for (UINT j = 0; j < columnVertexCount - 1; ++j)
        {
            const UINT currentIndex = i * columnVertexCount + j;
            const UINT nextIndex = currentIndex + 1;
            const UINT bottomIndex = currentIndex + columnVertexCount;
            const UINT bottomNextIndex = bottomIndex + 1;

            meshData.indices.push_back(currentIndex);
            meshData.indices.push_back(nextIndex);
            meshData.indices.push_back(bottomIndex);

            meshData.indices.push_back(bottomIndex);
            meshData.indices.push_back(nextIndex);
            meshData.indices.push_back(bottomNextIndex);
        }
    }

    return meshData;
}
