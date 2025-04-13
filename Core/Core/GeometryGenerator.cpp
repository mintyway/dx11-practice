#include "GeometryGenerator.h"

#include "Utilities/Utility.h"

#include <array>
#include <vector>
#include <cmath>
#include <iostream>

GeometryGenerator::MeshData GeometryGenerator::CreateBox(float width, float height, float depth)
{
    MeshData meshData;

    const float halfWidth = 0.5f * width;
    const float halfHeight = 0.5f * height;
    const float halfDepth = 0.5f * depth;

    meshData.vertices =
    {
        // 정면
        Vertex(XMFLOAT3(-halfWidth, -halfHeight, -halfDepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
        Vertex(XMFLOAT3(-halfWidth, +halfHeight, -halfDepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
        Vertex(XMFLOAT3(+halfWidth, +halfHeight, -halfDepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
        Vertex(XMFLOAT3(+halfWidth, -halfHeight, -halfDepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)),

        // 후면
        Vertex(XMFLOAT3(-halfWidth, -halfHeight, +halfDepth), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)),
        Vertex(XMFLOAT3(+halfWidth, -halfHeight, +halfDepth), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
        Vertex(XMFLOAT3(+halfWidth, +halfHeight, +halfDepth), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
        Vertex(XMFLOAT3(-halfWidth, +halfHeight, +halfDepth), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),

        // 윗면
        Vertex(XMFLOAT3(-halfWidth, +halfHeight, -halfDepth), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
        Vertex(XMFLOAT3(-halfWidth, +halfHeight, +halfDepth), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
        Vertex(XMFLOAT3(+halfWidth, +halfHeight, +halfDepth), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
        Vertex(XMFLOAT3(+halfWidth, +halfHeight, -halfDepth), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)),

        // 아랫면
        Vertex(XMFLOAT3(-halfWidth, -halfHeight, -halfDepth), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)),
        Vertex(XMFLOAT3(+halfWidth, -halfHeight, -halfDepth), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
        Vertex(XMFLOAT3(+halfWidth, -halfHeight, +halfDepth), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
        Vertex(XMFLOAT3(-halfWidth, -halfHeight, +halfDepth), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),

        // 좌측면
        Vertex(XMFLOAT3(-halfWidth, -halfHeight, +halfDepth), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f)),
        Vertex(XMFLOAT3(-halfWidth, +halfHeight, +halfDepth), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f)),
        Vertex(XMFLOAT3(-halfWidth, +halfHeight, -halfDepth), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f)),
        Vertex(XMFLOAT3(-halfWidth, -halfHeight, -halfDepth), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f)),

        // 우측면
        Vertex(XMFLOAT3(+halfWidth, -halfHeight, -halfDepth), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)),
        Vertex(XMFLOAT3(+halfWidth, +halfHeight, -halfDepth), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)),
        Vertex(XMFLOAT3(+halfWidth, +halfHeight, +halfDepth), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)),
        Vertex(XMFLOAT3(+halfWidth, -halfHeight, +halfDepth), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f))
    };

    meshData.indices =
    {
        // 정면
        0, 1, 2,
        0, 2, 3,

        // 후면
        4, 5, 6,
        4, 6, 7,

        // 윗면
        8, 9, 10,
        8, 10, 11,

        // 아랫면
        12, 13, 14,
        12, 14, 15,

        // 좌측면
        16, 17, 18,
        16, 18, 19,

        // 우측면
        20, 21, 22,
        20, 22, 23
    };

    return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::CreateSphere(float radius, UINT sliceCount, UINT stackCount)
{
    MeshData meshData;

    const Vertex topVertex(XMFLOAT3(0.0f, radius, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
    const Vertex bottomVertex(XMFLOAT3(0.0f, -radius, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));

    constexpr UINT poleVertexCount = 2;
    const UINT sideVertexCount = (stackCount - 1) * (sliceCount + 1);
    meshData.vertices.reserve(poleVertexCount + sideVertexCount);
    meshData.vertices.push_back(topVertex);

    const float phiStep = XM_PI / static_cast<float>(stackCount);
    const float thetaStep = XM_2PI / static_cast<float>(sliceCount);

    for (UINT i = 1; i <= stackCount - 1; ++i)
    {
        const float phi = static_cast<float>(i) * phiStep;
        const float sinPhi = std::sin(phi);
        const float cosPhi = std::cos(phi);

        for (UINT j = 0; j <= sliceCount; ++j)
        {
            const float theta = static_cast<float>(j) * thetaStep;
            const float sinTheta = std::sin(theta);
            const float cosTheta = std::cos(theta);

            Vertex vertex;

            vertex.position = XMFLOAT3(radius * sinPhi * cosTheta, radius * cosPhi, radius * sinPhi * sinTheta);

            XMStoreFloat3(&vertex.normal, XMVector3Normalize(XMLoadFloat3(&vertex.position)));

            vertex.tangentU = XMFLOAT3(-radius * sinPhi * sinTheta, 0.0f, radius * sinPhi * cosTheta);
            XMStoreFloat3(&vertex.tangentU, XMVector3Normalize(XMLoadFloat3(&vertex.tangentU)));

            vertex.texC = XMFLOAT2(theta / XM_2PI, phi / XM_PI);

            meshData.vertices.push_back(vertex);
        }
    }

    meshData.vertices.push_back(bottomVertex);

    const UINT capIndexCount = sliceCount * 3 * 2;
    const UINT sideIndexCount = sliceCount * (stackCount - 2) * 6;
    meshData.indices.reserve(capIndexCount + sideIndexCount);
    for (UINT i = 1; i <= sliceCount; ++i)
    {
        meshData.indices.push_back(0);
        meshData.indices.push_back(i + 1);
        meshData.indices.push_back(i);
    }

    const UINT ringVertexCount = sliceCount + 1;
    for (UINT i = 0; i < stackCount - 2; ++i)
    {
        for (UINT j = 0; j < sliceCount; ++j)
        {
            constexpr UINT topOffsetIndex = 1;
            const UINT currentIndex = topOffsetIndex + i * ringVertexCount + j;
            const UINT nextIndex = currentIndex + 1;
            const UINT bottomIndex = currentIndex + ringVertexCount;
            const UINT bottomNextIndex = bottomIndex + 1;

            meshData.indices.push_back(currentIndex);
            meshData.indices.push_back(nextIndex);
            meshData.indices.push_back(bottomIndex);

            meshData.indices.push_back(bottomIndex);
            meshData.indices.push_back(nextIndex);
            meshData.indices.push_back(bottomNextIndex);
        }
    }

    const UINT bottomIndex = static_cast<UINT>(meshData.vertices.size() - 1);

    const UINT bottomCapStartOffsetIndex = bottomIndex - ringVertexCount;

    for (UINT i = 0; i < sliceCount; ++i)
    {
        meshData.indices.push_back(bottomIndex);
        meshData.indices.push_back(bottomCapStartOffsetIndex + i);
        meshData.indices.push_back(bottomCapStartOffsetIndex + i + 1);
    }

    return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::CreateGeodesicSphere(float radius, UINT subdivisionCount)
{
    MeshData meshData;

    // 최대 분할은 5회로 제한한다. 5회만 되어도 이미 2만 폴리곤이 사용되므로 이 이상은 무의미하다.
    subdivisionCount = std::min(subdivisionCount, 5u);

    // 정이십면체를 테셀레이션해 구를 근사한다. 아래는 상수로 초기화하는 정이십면체이다.
    constexpr size_t icosahedronVertexCount = 12;
    constexpr size_t icosahedronIndexCount = 20ull * 3ull;

    constexpr float X = 0.525731f;
    constexpr float Z = 0.850651f;

    constexpr std::array<XMFLOAT3, icosahedronVertexCount> icosahedronVertices =
    {
        XMFLOAT3(-X, 0.0f, Z), XMFLOAT3(X, 0.0f, Z),
        XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
        XMFLOAT3(0.0f, Z, X), XMFLOAT3(0.0f, Z, -X),
        XMFLOAT3(0.0f, -Z, X), XMFLOAT3(0.0f, -Z, -X),
        XMFLOAT3(Z, X, 0.0f), XMFLOAT3(-Z, X, 0.0f),
        XMFLOAT3(Z, -X, 0.0f), XMFLOAT3(-Z, -X, 0.0f)
    };

    constexpr std::array<UINT, icosahedronIndexCount> icosahedronIndices =
    {
        1, 4, 0, 4, 9, 0, 4, 5, 9, 8, 5, 4, 1, 8, 4,
        1, 10, 8, 10, 3, 8, 8, 3, 5, 3, 2, 5, 3, 7, 2,
        3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6, 1, 0,
        10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2, 9, 11, 2, 7
    };

    meshData.vertices.resize(icosahedronVertexCount);
    meshData.indices.resize(icosahedronIndexCount);

    for (UINT i = 0; i < icosahedronVertexCount; ++i)
    {
        meshData.vertices[i].position = icosahedronVertices[i];
    }

    for (UINT i = 0; i < icosahedronIndexCount; ++i)
    {
        meshData.indices[i] = icosahedronIndices[i];
    }

    for (UINT i = 0; i < subdivisionCount; ++i)
    {
        Subdivide(meshData);
    }

    // 구 표면으로 정규화
    for (UINT i = 0; i < meshData.vertices.size(); ++i)
    {
        const XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&meshData.vertices[i].position));
        const XMVECTOR position = radius * normal;

        XMStoreFloat3(&meshData.vertices[i].position, position);
        XMStoreFloat3(&meshData.vertices[i].normal, normal);

        // 구면 좌표를 통해 텍스처 좌표를 구한다.
        const float theta = Math::AngleFromXY(meshData.vertices[i].position.x, meshData.vertices[i].position.z);

        const float phi = std::acos(meshData.vertices[i].position.y / radius);

        meshData.vertices[i].texC.x = theta / XM_2PI;
        meshData.vertices[i].texC.y = phi / XM_PI;

        // 세타에 대한 P의 편미분
        meshData.vertices[i].tangentU.x = -radius * std::sin(phi) * std::sin(theta);
        meshData.vertices[i].tangentU.y = 0.0f;
        meshData.vertices[i].tangentU.z = radius * std::sin(phi) * std::cos(theta);

        const XMVECTOR tangent = XMLoadFloat3(&meshData.vertices[i].tangentU);
        XMStoreFloat3(&meshData.vertices[i].tangentU, XMVector3Normalize(tangent));
    }

    return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount)
{
    MeshData meshData;

    const float stackHeight = height / static_cast<float>(stackCount);
    const float radiusStep = (topRadius - bottomRadius) / static_cast<float>(stackCount);
    const UINT ringCount = stackCount + 1;

    const float dTheta = XM_2PI / static_cast<float>(sliceCount);
    const float dRadius = bottomRadius - topRadius;

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

GeometryGenerator::MeshData GeometryGenerator::CreateGrid(float width, float depth, UINT rowVertexCount, UINT columnVertexCount)
{
    MeshData meshData;

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

void GeometryGenerator::Subdivide(MeshData& meshData)
{
    // 기존 데이터 백업
    MeshData inputCopy = meshData;

    // 초기화
    meshData.vertices.resize(0);
    meshData.indices.resize(0);

    const UINT triangleCount = static_cast<UINT>(inputCopy.indices.size() / 3);
    for (UINT i = 0; i < triangleCount; ++i)
    {
        //       v1
        //       *
        //      / \
        //     /   \
        //  m0*-----*m1
        //   / \   / \
        //  /   \ /   \
        // *-----*-----*
        // v0    m2     v2

        Vertex v0 = inputCopy.vertices[inputCopy.indices[i * 3 + 0]];
        Vertex v1 = inputCopy.vertices[inputCopy.indices[i * 3 + 1]];
        Vertex v2 = inputCopy.vertices[inputCopy.indices[i * 3 + 2]];

        // 중점 생성
        Vertex m0, m1, m2;
        m0.position = XMFLOAT3((v0.position.x + v1.position.x) * 0.5f, (v0.position.y + v1.position.y) * 0.5f, (v0.position.z + v1.position.z) * 0.5f);
        m1.position = XMFLOAT3((v1.position.x + v2.position.x) * 0.5f, (v1.position.y + v2.position.y) * 0.5f, (v1.position.z + v2.position.z) * 0.5f);
        m2.position = XMFLOAT3((v0.position.x + v2.position.x) * 0.5f, (v0.position.y + v2.position.y) * 0.5f, (v0.position.z + v2.position.z) * 0.5f);

        // 분할된 버텍스와 인덱스를 추가
        meshData.vertices.push_back(v0); // 0
        meshData.vertices.push_back(v1); // 1
        meshData.vertices.push_back(v2); // 2
        meshData.vertices.push_back(m0); // 3
        meshData.vertices.push_back(m1); // 4
        meshData.vertices.push_back(m2); // 5

        meshData.indices.push_back(i * 6 + 0);
        meshData.indices.push_back(i * 6 + 3);
        meshData.indices.push_back(i * 6 + 5);

        meshData.indices.push_back(i * 6 + 3);
        meshData.indices.push_back(i * 6 + 4);
        meshData.indices.push_back(i * 6 + 5);

        meshData.indices.push_back(i * 6 + 5);
        meshData.indices.push_back(i * 6 + 4);
        meshData.indices.push_back(i * 6 + 2);

        meshData.indices.push_back(i * 6 + 3);
        meshData.indices.push_back(i * 6 + 1);
        meshData.indices.push_back(i * 6 + 4);
    }
}
