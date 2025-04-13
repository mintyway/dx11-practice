#pragma once
//***************************************************************************************
// Waves.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs the calculations for the wave simulation.  After the simulation has been
// updated, the client must copy the current solution into vertex buffers for rendering.
// This class only does the calculations, it does not do any drawing.
//***************************************************************************************

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

/** 해당 클래스는 아직 분석하지 않았습니다. */
class Waves
{
public:
    Waves() = default;
    ~Waves();

    UINT RowCount() const { return mNumRows; }
    UINT ColumnCount() const { return mNumCols; }
    UINT VertexCount() const { return mVertexCount; }
    UINT TriangleCount() const { return mTriangleCount; }
    float Width() const { return static_cast<float>(mNumCols) * mSpatialStep; }
    float Depth() const { return static_cast<float>(mNumRows) * mSpatialStep; }

    // Returns the solution at the ith grid point.
    const XMFLOAT3& operator[](int i) const { return mCurrSolution[i]; }

    // Returns the solution normal at the ith grid point.
    const XMFLOAT3& Normal(int i) const { return mNormals[i]; }

    // Returns the unit tangent vector at the ith grid point in the local x-axis direction.
    const XMFLOAT3& TangentX(int i) const { return mTangentX[i]; }

    void Init(UINT m, UINT n, float dx, float dt, float speed, float damping);
    void Update(float dt);
    void Disturb(UINT i, UINT j, float magnitude);

private:
    UINT mNumRows = 0;
    UINT mNumCols = 0;

    UINT mVertexCount = 0;
    UINT mTriangleCount = 0;

    // Simulation constants we can precompute.
    float mK1 = 0.0f;
    float mK2 = 0.0f;
    float mK3 = 0.0f;

    float mTimeStep = 0.0f;
    float mSpatialStep = 0.0f;

    XMFLOAT3* mPrevSolution = nullptr;
    XMFLOAT3* mCurrSolution = nullptr;
    XMFLOAT3* mNormals = nullptr;
    XMFLOAT3* mTangentX = nullptr;
};
