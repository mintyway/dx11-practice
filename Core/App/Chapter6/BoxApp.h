#pragma once

#include "Core/BaseEngine.h"
#include "Rendering/VertexTypes.h"

#include <vector>

using namespace DirectX;

class BoxApp : public BaseEngine
{
    DECLARE_ENGINE(BoxApp, BaseEngine)

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

    XMFLOAT4X4 worldMatrix;
    XMFLOAT4X4 viewMatrix;
    XMFLOAT4X4 projectionMatrix;

    float radius = 5.0f;
    float theta = -XM_PIDIV4;
    float phi = 0.25f * XM_PI;

    POINT lastMousePosition{};

    std::vector<UINT> boxIndices;
};
