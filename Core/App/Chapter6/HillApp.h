#pragma once

#include "Core/Engine/EngineBase.h"
#include "Rendering/VertexTypes.h"

class HillApp : public EngineBase
{
    DECLARE_ENGINE(HillApp, EngineBase)

public:
    ~HillApp() override = default;

    static float GetHeight(float x, float z) { return 0.3f * (z * std::sin(0.1f * x) + x * std::cos(0.1f * z)); }

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;
    void OnMouseDown(WPARAM buttonState, int x, int y) override;
    void OnMouseUp(WPARAM buttonState, int x, int y) override;
    void OnMouseMove(WPARAM buttonState, int x, int y) override;

protected:
    HillApp();

    void Update(float deltaSeconds) override;
    virtual void Render() override;

private:
    void CreateGeometryBuffers();
    void CreateShaders();

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    UINT indexCount = 0;

    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;

    ComPtr<ID3D11Buffer> constantBuffer;

    DirectX::XMFLOAT4X4 worldMatrix;
    DirectX::XMFLOAT4X4 viewMatrix;
    DirectX::XMFLOAT4X4 projectionMatrix;

    float radius = 200.0f;
    float theta = -DirectX::XM_PIDIV4;
    float phi = 0.25f * DirectX::XM_PI;

    POINT lastMousePosition{};
};
