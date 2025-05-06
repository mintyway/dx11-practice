#pragma once

#include <array>

#include "Core/Engine/SphericalCamera.h"
#include "Core/Light/Light.h"
#include "Core/Rendering/Submesh.h"
#include "Core/Utilities/Waves.h"

class BlendDemoShaderPass;

class BlendDemoApp final : public SphericalCamera
{
    DECLARE_ENGINE(BlendDemoApp, SphericalCamera)

public:
    BlendDemoApp();
    ~BlendDemoApp() override;

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;

protected:
    virtual void Update(float deltaSeconds) override;
    virtual void Render() override;

    void RenderObject(ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr, ID3D11RasterizerState* rasterizerState, ID3D11BlendState* blendState, DirectX::XMMATRIX worldMatrix, DirectX::CXMMATRIX viewProjectionMatrix, DirectX::CXMMATRIX uvMatrix, ID3D11ShaderResourceView* diffuseMapSRV, const Material& material, UINT indexCount);

private:
    void CreateGeometry();
    void CreateLandGeometry();
    void CreateWaveGeometry();
    void CreateWireFenceGeometry();

    void InitTexture();

    static float GetHeight(float x, float z) { return 0.3f * (z * std::sin(0.1f * x) + x * std::cos(0.1f * z)); }

    static DirectX::XMFLOAT3 GetHillNormal(float x, float z)
    {
        DirectX::XMFLOAT3 n(-0.03f * z * std::cos(0.1f * x) - 0.3f * std::cos(0.1f * z), 1.0f, -0.3f * std::sin(0.1f * x) + 0.03f * x * std::sin(0.1f * z));
        XMStoreFloat3(&n, DirectX::XMVector3Normalize(XMLoadFloat3(&n)));
        return n;
    }

    std::unique_ptr<BlendDemoShaderPass> shaderPass;
    ComPtr<ID3D11SamplerState> samplerState;

    DirectX::XMFLOAT4X4 viewMatrix;
    DirectX::XMFLOAT4X4 projectionMatrix;
    DirectX::XMFLOAT3 eyePosition;

    std::array<DirectionalLight, 3> directionalLights;

    ComPtr<ID3D11Buffer> hillsVertexBuffer;
    ComPtr<ID3D11Buffer> hillsIndexBuffer;
    ComPtr<ID3D11ShaderResourceView> hillsDiffuseMapSRV;
    DirectX::XMFLOAT4X4 hillsWorldMatrix;
    DirectX::XMFLOAT4X4 hillsUVMatrix;
    Material hillsMaterial;
    Submesh hillsSubmesh{};

    Waves waves;
    ComPtr<ID3D11Buffer> wavesVertexBuffer;
    ComPtr<ID3D11Buffer> wavesIndexBuffer;
    ComPtr<ID3D11ShaderResourceView> wavesDiffuseMapSRV;
    DirectX::XMFLOAT4X4 wavesWorldMatrix;
    DirectX::XMFLOAT4X4 wavesUVMatrix;
    Material wavesMaterial;
    Submesh wavesSubmesh{};

    ComPtr<ID3D11Buffer> wireFenceVertexBuffer;
    ComPtr<ID3D11Buffer> wireFenceIndexBuffer;
    ComPtr<ID3D11ShaderResourceView> wireFenceDiffuseMapSRV;
    DirectX::XMFLOAT4X4 wireFenceWorldMatrix;
    DirectX::XMFLOAT4X4 wireFenceUVMatrix;
    Material wireFenceMaterial;
    Submesh wireFenceSubmesh{};
};
