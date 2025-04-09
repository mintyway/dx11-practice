#pragma once

#include "Core/BaseEngine.h"
#include "Rendering/Submesh.h"
#include "Rendering/VertexTypes.h"

#include <array>

using namespace DirectX;

class MultiDrawApp : public BaseEngine
{
    DECLARE_ENGINE(MultiDrawApp, BaseEngine)

public:
    ~MultiDrawApp() override = default;

    static float GetHeight(float x, float z) { return 0.3f * (z * std::sin(0.1f * x) + x * std::cos(0.1f * z)); }

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;
    void OnMouseDown(WPARAM buttonState, int x, int y) override;
    void OnMouseUp(WPARAM buttonState, int x, int y) override;
    void OnMouseMove(WPARAM buttonState, int x, int y) override;

protected:
    MultiDrawApp();

    void Update(float deltaSeconds) override;
    virtual void Render() override;

private:
    void CreateGeometryBuffers();
    void CreateShaders();
    void BindShader();

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    UINT indexCount = 0;

    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;

    ComPtr<ID3D11Buffer> constantBuffer;

    ComPtr<ID3D11RasterizerState> wireframeRasterizerState;

    XMFLOAT4X4 viewMatrix;
    XMFLOAT4X4 projectionMatrix;

    float radius = 30.0f;
    float theta = -XM_PIDIV4;
    float phi = 0.25f * XM_PI;

    POINT lastMousePosition{};

    std::array<XMFLOAT4X4, 10> sphereWorldMatrices;
    std::array<XMFLOAT4X4, 10> cylinderWorldMatrices;
    XMFLOAT4X4 boxWorldMatrix;
    XMFLOAT4X4 gridWorldMatrix;
    XMFLOAT4X4 centerSphereWorldMatrix;

    Submesh boxSubmesh{};
    Submesh gridSubmesh{};
    Submesh sphereSubmesh{};
    Submesh cylinderSubmesh{};
};
