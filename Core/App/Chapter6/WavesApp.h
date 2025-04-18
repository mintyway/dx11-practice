#pragma once

#include "Core/Engine/EngineBase.h"
#include "Rendering/Submesh.h"
#include "Rendering/VertexTypes.h"
#include "Utilities/Waves.h"

class WavesApp : public EngineBase
{
    DECLARE_ENGINE(WavesApp, EngineBase)

public:
    ~WavesApp() override = default;

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;
    virtual void OnMouseDown(WPARAM buttonState, int x, int y) override;
    virtual void OnMouseUp(WPARAM buttonState, int x, int y) override;
    virtual void OnMouseMove(WPARAM buttonState, int x, int y) override;

protected:
    WavesApp();

    virtual void Update(float deltaSeconds) override;
    virtual void Render() override;

private:
    bool CreateShaders();
    bool BindShader();
    bool CreateGeometryBuffers();

    bool CreateLandGeometryBuffer();
    bool CreateWaveGeometryBuffer();

    static float GetHeight(float x, float z) { return 0.3f * (z * std::sin(0.1f * x) + x * std::cos(0.1f * z)); }

    ComPtr<ID3D11InputLayout> inputLayout;
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;

    ComPtr<ID3D11Buffer> constantBuffer;

    ComPtr<ID3D11Buffer> landVertexBuffer;
    ComPtr<ID3D11Buffer> landIndexBuffer;

    ComPtr<ID3D11Buffer> wavesVertexBuffer;
    ComPtr<ID3D11Buffer> wavesIndexBuffer;

    float radius = 200.0f;
    float theta = -DirectX::XM_PIDIV4;
    float phi = DirectX::XM_PIDIV4;

    POINT lastMousePosition{};

    DirectX::XMFLOAT4X4 viewMatrix;
    DirectX::XMFLOAT4X4 projectionMatrix;

    DirectX::XMFLOAT4X4 landWorldMatrix;
    DirectX::XMFLOAT4X4 wavesWorldMatrix;

    Waves waves;

    Submesh gridSubmesh{};
    Submesh wavesSubmesh{};
};
