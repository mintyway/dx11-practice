#pragma once

#include <d3d11.h>

struct MeshData;

class GeometryGenerator
{
public:
    static void CreateGrid(float width, float depth, UINT rowVertexCount, UINT columnVertexCount, MeshData& meshData);
};
