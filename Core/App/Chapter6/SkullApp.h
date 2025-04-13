#pragma once

#include "Core/SphericalCamera.h"

class SkullApp : public SphericalCamera
{
    DECLARE_ENGINE(SkullApp, SphericalCamera)

public:
    ~SkullApp() override = default;

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;

protected:
    SkullApp();

    void Update(float deltaSeconds) override;
    void Render() override;

private:
    void LoadSkullMesh();
    void InitShaderResource();

    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;

    ComPtr<ID3D11InputLayout> inputLayout;

    ComPtr<ID3D11Buffer> objectCB;

    ComPtr<ID3D11Buffer> skullVertexBuffer;
    ComPtr<ID3D11Buffer> skullIndexBuffer;

    XMFLOAT4X4 viewMatrix{};
    XMFLOAT4X4 projectionMatrix{};
    
    XMFLOAT4X4 skullWolrdMatrix{};
    UINT skullIndexCount;
};
