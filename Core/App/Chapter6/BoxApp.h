#pragma once

#include <vector>

#include "Engine/EngineBase.h"
#include "Rendering/VertexTypes.h"

class BoxApp : public EngineBase
{
    DECLARE_ENGINE(BoxApp, EngineBase)

public:
    ~BoxApp() override = default;

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;
    void OnMouseDown(WPARAM buttonState, int x, int y) override;
    void OnMouseUp(WPARAM buttonState, int x, int y) override;
    void OnMouseMove(WPARAM buttonState, int x, int y) override;

protected:
    BoxApp();

    void Update(float deltaSeconds) override;
    virtual void Render() override;

private:
    void CreateGeometryBuffers();
    void CreateShaders();

    ComPtr<ID3D11Buffer> boxPositionVertexBuffer;
    ComPtr<ID3D11Buffer> boxColorVertexBuffer;
    ComPtr<ID3D11Buffer> boxIndexBuffer;

    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;

    ComPtr<ID3D11Buffer> wvpMatrixBuffer;
    ComPtr<ID3D11Buffer> timeBuffer;

    DirectX::XMFLOAT4X4 worldMatrix;
    DirectX::XMFLOAT4X4 viewMatrix;
    DirectX::XMFLOAT4X4 projectionMatrix;

    float radius = 5.0f;
    float theta = -DirectX::XM_PIDIV4;
    float phi = 0.25f * DirectX::XM_PI;

    POINT lastMousePosition{};

    std::vector<UINT> boxIndices;
};
