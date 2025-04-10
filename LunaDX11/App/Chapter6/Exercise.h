#pragma once

#include "Core/BaseEngine.h"

#include <array>

using namespace DirectX;

class Exercise : public BaseEngine
{
    DECLARE_ENGINE(Exercise, BaseEngine)

public:
    ~Exercise() override = default;

    bool Init(HINSTANCE inInstanceHandle) override;
    LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
    void OnResize() override;

    void OnMouseDown(WPARAM buttonState, int x, int y) override;
    void OnMouseUp(WPARAM buttonState, int x, int y) override;
    void OnMouseMove(WPARAM buttonState, int x, int y) override;

protected:
    Exercise();

    void Update(float deltaSeconds) override;
    void Render() override;

private:
    void InitShader();
    void CreateGeometry();
    void CreateGeodesicSphere();
    void DrawObject(ID3D11Buffer* inVertexBuffer, ID3D11Buffer* inIndexBuffer, const XMFLOAT4X4& inWorldMatrix, UINT inIndexCount);

    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;

    ComPtr<ID3D11InputLayout> inputLayout;

    ComPtr<ID3D11Buffer> wvpMatrixBuffer;

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    UINT vertexCount = 0;

    XMFLOAT4X4 worldMatrix;
    XMFLOAT4X4 viewMatrix;
    XMFLOAT4X4 projectionMatrix;

    float radius = 10.0f;
    float theta = -XM_PIDIV2;
    float phi = XM_PIDIV2;

    POINT lastMousePosition{};

    std::array<XMFLOAT3, 8> pointPositions; 
    ComPtr<ID3D11Buffer> geodesicSphereVertexBuffer;
    ComPtr<ID3D11Buffer> geodesicSphereIndexBuffer;
    XMFLOAT4X4 boxWolrdMatrix;
    UINT geodesicSphereIndexCount = 0;
};
