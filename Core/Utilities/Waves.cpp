//***************************************************************************************
// Waves.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Waves.h"
#include <algorithm>
#include <vector>
#include <cassert>

using namespace DirectX;

Waves::~Waves()
{
    delete[] mPrevSolution;
    delete[] mCurrSolution;
    delete[] mNormals;
    delete[] mTangentX;
}

void Waves::Init(UINT m, UINT n, float dx, float dt, float speed, float damping)
{
    mNumRows = m;
    mNumCols = n;

    mVertexCount = m * n;
    mTriangleCount = (m - 1) * (n - 1) * 2;

    mTimeStep = dt;
    mSpatialStep = dx;

    const float d = damping * dt + 2.0f;
    const float e = (speed * speed) * (dt * dt) / (dx * dx);
    mK1 = (damping * dt - 2.0f) / d;
    mK2 = (4.0f - 8.0f * e) / d;
    mK3 = (2.0f * e) / d;

    // In case Init() called again.
    delete[] mPrevSolution;
    delete[] mCurrSolution;
    delete[] mNormals;
    delete[] mTangentX;

    mPrevSolution = new XMFLOAT3[static_cast<size_t>(m) * n];
    mCurrSolution = new XMFLOAT3[static_cast<size_t>(m) * n];
    mNormals = new XMFLOAT3[static_cast<size_t>(m) * n];
    mTangentX = new XMFLOAT3[static_cast<size_t>(m) * n];

    // Generate grid vertices in system memory.

    const float halfWidth = static_cast<float>(n - 1) * dx * 0.5f;
    const float halfDepth = static_cast<float>(m - 1) * dx * 0.5f;
    for (UINT i = 0; i < m; ++i)
    {
        const float z = halfDepth - static_cast<float>(i) * dx;
        for (UINT j = 0; j < n; ++j)
        {
            const float x = -halfWidth + static_cast<float>(j) * dx;

            mPrevSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
            mCurrSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
            mNormals[i * n + j] = XMFLOAT3(0.0f, 1.0f, 0.0f);
            mTangentX[i * n + j] = XMFLOAT3(1.0f, 0.0f, 0.0f);
        }
    }
}

void Waves::Update(float dt)
{
    static float time = 0;

    // Accumulate time.
    time += dt;

    // Only update the simulation at the specified time step.
    if (time >= mTimeStep)
    {
        // Only update interior points; we use zero boundary conditions.
        for (UINT i = 1; i < mNumRows - 1; ++i)
        {
            for (UINT j = 1; j < mNumCols - 1; ++j)
            {
                // After this update we will be discarding the old previous
                // buffer, so overwrite that buffer with the new update.
                // Note how we can do this inplace (read/write to same element) 
                // because we won't need prev_ij again and the assignment happens last.

                // Note j indexes x and i indexes z: h(x_j, z_i, t_k)
                // Moreover, our +z axis goes "down"; this is just to 
                // keep consistent with our row indices going down.

                mPrevSolution[i * mNumCols + j].y =
                    mK1 * mPrevSolution[i * mNumCols + j].y +
                    mK2 * mCurrSolution[i * mNumCols + j].y +
                    mK3 * (mCurrSolution[(i + 1) * mNumCols + j].y +
                        mCurrSolution[(i - 1) * mNumCols + j].y +
                        mCurrSolution[i * mNumCols + j + 1].y +
                        mCurrSolution[i * mNumCols + j - 1].y);
            }
        }

        // We just overwrote the previous buffer with the new data, so
        // this data needs to become the current solution and the old
        // current solution becomes the new previous solution.
        std::swap(mPrevSolution, mCurrSolution);

        time = 0.0f; // reset time

        //
        // Compute normals using finite difference scheme.
        //
        for (UINT i = 1; i < mNumRows - 1; ++i)
        {
            for (UINT j = 1; j < mNumCols - 1; ++j)
            {
                const float l = mCurrSolution[i * mNumCols + j - 1].y;
                const float r = mCurrSolution[i * mNumCols + j + 1].y;
                const float t = mCurrSolution[(i - 1) * mNumCols + j].y;
                const float b = mCurrSolution[(i + 1) * mNumCols + j].y;
                mNormals[i * mNumCols + j].x = -r + l;
                mNormals[i * mNumCols + j].y = 2.0f * mSpatialStep;
                mNormals[i * mNumCols + j].z = b - t;

                const XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mNormals[i * mNumCols + j]));
                XMStoreFloat3(&mNormals[i * mNumCols + j], n);

                mTangentX[i * mNumCols + j] = XMFLOAT3(2.0f * mSpatialStep, r - l, 0.0f);
                const XMVECTOR T = XMVector3Normalize(XMLoadFloat3(&mTangentX[i * mNumCols + j]));
                XMStoreFloat3(&mTangentX[i * mNumCols + j], T);
            }
        }
    }
}

void Waves::Disturb(UINT i, UINT j, float magnitude)
{
    // Don't disturb boundaries.
    assert(i > 1 && i < mNumRows-2);
    assert(j > 1 && j < mNumCols-2);

    const float halfMag = 0.5f * magnitude;

    // Disturb the ijth vertex height and its neighbors.
    mCurrSolution[i * mNumCols + j].y += magnitude;
    mCurrSolution[i * mNumCols + j + 1].y += halfMag;
    mCurrSolution[i * mNumCols + j - 1].y += halfMag;
    mCurrSolution[(i + 1) * mNumCols + j].y += halfMag;
    mCurrSolution[(i - 1) * mNumCols + j].y += halfMag;
}
