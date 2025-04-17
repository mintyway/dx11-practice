#pragma once

#include "Core/Core/SphericalCamera.h"
#include "Core/Light/Light.h"
#include "Core/Rendering/Submesh.h"

#include <array>

class LitSkullApp final : public SphericalCamera
{
    DECLARE_ENGINE(LitSkullApp, SphericalCamera)

public:
    ~LitSkullApp() override = default;

    bool Init(HINSTANCE inInstanceHandle) override;
    LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
    void OnResize() override;

protected:
    LitSkullApp();

    void Update(float deltaSeconds) override;
    void Render() override;

private:
    ComPtr<ID3D11InputLayout> inputLayout;
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;

    ComPtr<ID3D11Buffer> objectCB;
    ComPtr<ID3D11Buffer> sceneCB;

    XMFLOAT4X4 viewProjectionMatrix{};

    std::array<DirectionalLight, 3> directionalLights{};

    ComPtr<ID3D11Buffer> skullVertexBuffer;
    ComPtr<ID3D11Buffer> skullIndexBuffer;
    Submesh skullSubmesh{};

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    Submesh boxSubmesh{};
    Submesh gridSubmesh{};
    Submesh sphereSubmesh{};
    Submesh cylinderSubmesh{};

    XMFLOAT4X4 skullWorldMatrix{};
    XMFLOAT4X4 boxWorldMatrix{};
    XMFLOAT4X4 gridWorldMatrix{};
    std::array<XMFLOAT4X4, 10> sphereWorldMatrices{};
    std::array<XMFLOAT4X4, 10> cylinderMatrices{};

    Material skullMaterial{};
    Material BoxMaterial{};
    Material GridMaterial{};
    Material SphereMaterial{};
    Material CylinderMaterial{};
};
