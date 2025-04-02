#pragma once

#include "VertexTypes.h"
#include <vector>

struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
};
