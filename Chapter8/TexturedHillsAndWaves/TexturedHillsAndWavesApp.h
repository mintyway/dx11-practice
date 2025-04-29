#pragma once

#include "Core/Engine/SphericalCamera.h"
#include "Core/Light/Light.h"
#include "Core/Rendering/Submesh.h"
#include "Core/Utilities/Waves.h"

class TexturedHillsAndWavesShaderPass;

class TexturedHillsAndWavesApp final : public SphericalCamera
{
    DECLARE_ENGINE(TexturedHillsAndWavesApp, SphericalCamera)

public:
    TexturedHillsAndWavesApp();
    ~TexturedHillsAndWavesApp() override;

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;

protected:
    virtual void Update(float deltaSeconds) override;
    virtual void Render() override;

    void RenderObject(ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr, DirectX::FXMMATRIX worldMatrix, DirectX::CXMMATRIX viewProjectionMatrix, const Material& material, UINT indexCount);

private:
    bool CreateGeometry();

    bool CreateLandGeometry();
    bool CreateWaveGeometry();

    static float GetHeight(float x, float z) { return 0.3f * (z * std::sin(0.1f * x) + x * std::cos(0.1f * z)); }

    static DirectX::XMFLOAT3 GetHillNormal(float x, float z)
    {
        DirectX::XMFLOAT3 n(-0.03f * z * std::cos(0.1f * x) - 0.3f * std::cos(0.1f * z), 1.0f, -0.3f * std::sin(0.1f * x) + 0.03f * x * std::sin(0.1f * z));
        XMStoreFloat3(&n, DirectX::XMVector3Normalize(XMLoadFloat3(&n)));
        return n;
    }

    std::unique_ptr<TexturedHillsAndWavesShaderPass> basicShader;

    ComPtr<ID3D11Buffer> landVertexBuffer;
    ComPtr<ID3D11Buffer> landIndexBuffer;

    ComPtr<ID3D11Buffer> wavesVertexBuffer;
    ComPtr<ID3D11Buffer> wavesIndexBuffer;

    DirectX::XMFLOAT4X4 viewMatrix;
    DirectX::XMFLOAT4X4 projectionMatrix;

    DirectX::XMFLOAT4X4 landWorldMatrix;
    DirectX::XMFLOAT4X4 wavesWorldMatrix;

    DirectX::XMFLOAT3 eyePosition;

    DirectionalLight directionalLight;
    PointLight pointLight;
    SpotLight spotLight;
    Material landMaterial;
    Material wavesMaterial;

    Waves waves;

    Submesh gridSubmesh{};
    Submesh wavesSubmesh{};
};
