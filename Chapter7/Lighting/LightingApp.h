#pragma once

#include "Core/Core/SphericalCamera.h"
#include "Core/Rendering/Submesh.h"
#include "Core/Utilities/Waves.h"

using namespace DirectX;

class LightingApp : public SphericalCamera
{
    DECLARE_ENGINE(LightingApp, SphericalCamera)

public:
    ~LightingApp() override = default;

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;

protected:
    LightingApp();

    virtual void Update(float deltaSeconds) override;
    virtual void Render() override;

    void RenderObject(ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr, const XMFLOAT4X4& worldMatrix, XMMATRIX viewProjectionMatrix, UINT indexCount, bool bUseWireframe = false);

private:
    bool CreateGeometry();
    bool InitShaderResource();

    bool CreateLandGeometry();
    bool CreateWaveGeometry();

    static float GetHeight(float x, float z) { return 0.3f * (z * std::sin(0.1f * x) + x * std::cos(0.1f * z)); }

    ComPtr<ID3D11InputLayout> inputLayout;
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;

    ComPtr<ID3D11Buffer> objectConstantBuffer;

    ComPtr<ID3D11Buffer> landVertexBuffer;
    ComPtr<ID3D11Buffer> landIndexBuffer;

    ComPtr<ID3D11Buffer> wavesVertexBuffer;
    ComPtr<ID3D11Buffer> wavesIndexBuffer;

    XMFLOAT4X4 viewMatrix;
    XMFLOAT4X4 projectionMatrix;

    XMFLOAT4X4 landWorldMatrix;
    XMFLOAT4X4 wavesWorldMatrix;

    Waves waves;

    Submesh gridSubmesh{};
    Submesh wavesSubmesh{};
};
