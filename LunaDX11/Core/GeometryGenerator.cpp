#include "GeometryGenerator.h"

#include <vector>
#include <cmath>
#include <iostream>

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

GeometryGenerator::MeshData GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount)
{
    const float stackHeight = height / static_cast<float>(stackCount);
    const float radiusStep = (topRadius - bottomRadius) / static_cast<float>(stackCount);
    const UINT ringCount = stackCount + 1;

    const float dTheta = XM_2PI / static_cast<float>(sliceCount);
    const float dRadius = bottomRadius - topRadius;

    MeshData meshData;

    const UINT sideVertexCount = (sliceCount + 1) * ringCount;
    const UINT capVertexCount = (sliceCount + 2) * 2;
    meshData.vertices.reserve(sideVertexCount + capVertexCount);

    for (UINT i = 0; i < ringCount; ++i)
    {
        const float currentHeight = -0.5f * height + static_cast<float>(i) * stackHeight;
        const float currentRadius = bottomRadius + static_cast<float>(i) * radiusStep;

        for (UINT j = 0; j <= sliceCount; ++j) // 시작점과 끝점을 같은 버텍스로 써버리면 텍스처 보간에 문제가 생기니 끝점을 시작점 위치에 하나 더 생성하고 uv좌표만 다르게 해준다. 
        {
            Vertex vertex;

            const float cos = std::cos(static_cast<float>(j) * dTheta);
            const float sin = std::sin(static_cast<float>(j) * dTheta);
            vertex.position = XMFLOAT3(currentRadius * cos, currentHeight, currentRadius * sin);

            vertex.texC = XMFLOAT2(static_cast<float>(j) / static_cast<float>(sliceCount), 1.0f - static_cast<float>(i) / static_cast<float>(stackCount));
            vertex.tangentU = XMFLOAT3(-sin, 0.0f, cos);

            XMFLOAT3 bitangent(dRadius * cos, -height, dRadius * sin);

            const XMVECTOR t = XMLoadFloat3(&vertex.tangentU);
            const XMVECTOR b = XMLoadFloat3(&bitangent);
            const XMVECTOR n = XMVector3Normalize(XMVector3Cross(t, b));
            XMStoreFloat3(&vertex.normal, n);

            meshData.vertices.push_back(vertex);
        }
    }

    const UINT sideIndexCount = sliceCount * stackCount * 6;
    const UINT capIndexCount = sliceCount * 3 * 2;
    meshData.indices.reserve(sideIndexCount + capIndexCount);

    for (UINT i = 0; i < ringCount - 1; ++i)
    {
        for (UINT j = 0; j < sliceCount; ++j)
        {
            const UINT currentIndex = i * (sliceCount + 1) + j;
            const UINT topIndex = currentIndex + (sliceCount + 1);
            const UINT topNextIndex = topIndex + 1;
            const UINT nextIndex = currentIndex + 1;

            meshData.indices.push_back(currentIndex);
            meshData.indices.push_back(topIndex);
            meshData.indices.push_back(topNextIndex);

            meshData.indices.push_back(currentIndex);
            meshData.indices.push_back(topNextIndex);
            meshData.indices.push_back(nextIndex);
        }
    }

    auto AddCap = [&](float radius, bool isTop)
    {
        const UINT StartIndexOffset = static_cast<UINT>(meshData.vertices.size());
        const float y = isTop ? height * 0.5f : height * -0.5f;

        const XMFLOAT3 normal = isTop ? XMFLOAT3(0.0f, 1.0f, 0.0f) : XMFLOAT3(0.0f, -1.0f, 0.0f);
        constexpr XMFLOAT3 tangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);

        for (UINT i = 0; i <= sliceCount; ++i)
        {
            Vertex vertex;

            const float x = radius * std::cos(static_cast<float>(i) * dTheta);
            const float z = radius * std::sin(static_cast<float>(i) * dTheta);
            vertex.position = XMFLOAT3(x, y, z);

            vertex.normal = normal;
            vertex.tangentU = tangentU;

            const float u = x * 0.5f / radius + 0.5f;
            const float v = z * 0.5f / radius + 0.5f;
            vertex.texC = XMFLOAT2(u, v);

            meshData.vertices.push_back(vertex);
        }

        Vertex centerVertex;
        centerVertex.position = XMFLOAT3(0.0f, y, 0.0f);
        centerVertex.normal = normal;
        centerVertex.tangentU = tangentU;
        centerVertex.texC = XMFLOAT2(0.5f, 0.5f);
        meshData.vertices.push_back(centerVertex);

        const UINT centerIndex = static_cast<UINT>(meshData.vertices.size() - 1);

        if (isTop)
        {
            for (UINT i = 0; i < sliceCount; ++i)
            {
                meshData.indices.push_back(centerIndex);
                meshData.indices.push_back(StartIndexOffset + i + 1);
                meshData.indices.push_back(StartIndexOffset + i);
            }
        }
        else
        {
            for (UINT i = 0; i < sliceCount; ++i)
            {
                meshData.indices.push_back(centerIndex);
                meshData.indices.push_back(StartIndexOffset + i);
                meshData.indices.push_back(StartIndexOffset + i + 1);
            }
        }
    };

    AddCap(topRadius, true);
    AddCap(bottomRadius, false);

    return meshData;
}
