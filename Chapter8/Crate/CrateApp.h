#pragma once
#include <array>

#include "Core/Engine/SphericalCamera.h"
#include "Core/Light/Light.h"

class CrateShaderPass;

class CrateApp final : public SphericalCamera
{
    DECLARE_ENGINE(CrateApp, SphericalCamera)

public:
    CrateApp();
    ~CrateApp() override;

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;

protected:
    void Update(float deltaSeconds) override;
    void Render() override;

    void InitGeometry();
    void InitTexture();

    std::unique_ptr<CrateShaderPass> shaderPass;

    ComPtr<ID3D11ShaderResourceView> diffuseMapSRV;
    DirectX::XMFLOAT4X4 TextureTransformMatrix;

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    UINT indexCount = 0;

    DirectX::XMFLOAT4X4 WorldMatrix{};
    DirectX::XMFLOAT4X4 viewMatrix{};
    DirectX::XMFLOAT4X4 projectionMatrix{};
    DirectX::XMFLOAT3 eyePosition{};

    const std::array<DirectionalLight, 3> DirectionalLights;
    UINT activeLightCount = static_cast<UINT>(DirectionalLights.size());

    Material material;
};
