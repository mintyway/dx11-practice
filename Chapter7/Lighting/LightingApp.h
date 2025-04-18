#pragma once

#include "Core/Core/SphericalCamera.h"
#include "Core/Rendering/Submesh.h"
#include "Core/Utilities/Waves.h"
#include "Core/Light/Light.h"

using namespace DirectX;

class BasicShader;

class LightingApp final : public SphericalCamera
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

    void RenderObject(ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr, FXMMATRIX worldMatrix, CXMMATRIX viewProjectionMatrix, const Material& material, UINT indexCount);

private:
    bool CreateGeometry();

    bool CreateLandGeometry();
    bool CreateWaveGeometry();

    static float GetHeight(float x, float z) { return 0.3f * (z * std::sin(0.1f * x) + x * std::cos(0.1f * z)); }

    static XMFLOAT3 GetHillNormal(float x, float z)
    {
        XMFLOAT3 n(-0.03f * z * std::cos(0.1f * x) - 0.3f * std::cos(0.1f * z), 1.0f, -0.3f * std::sin(0.1f * x) + 0.03f * x * std::sin(0.1f * z));
        XMStoreFloat3(&n, XMVector3Normalize(XMLoadFloat3(&n)));
        return n;
    }

    std::unique_ptr<BasicShader> basicShader;

    ComPtr<ID3D11Buffer> landVertexBuffer;
    ComPtr<ID3D11Buffer> landIndexBuffer;

    ComPtr<ID3D11Buffer> wavesVertexBuffer;
    ComPtr<ID3D11Buffer> wavesIndexBuffer;

    XMFLOAT4X4 viewMatrix;
    XMFLOAT4X4 projectionMatrix;

    XMFLOAT4X4 landWorldMatrix;
    XMFLOAT4X4 wavesWorldMatrix;

    XMFLOAT3 eyePosition;

    DirectionalLight directionalLight;
    PointLight pointLight;
    SpotLight spotLight;
    Material landMaterial;
    Material wavesMaterial;

    Waves waves;

    Submesh gridSubmesh{};
    Submesh wavesSubmesh{};
};
