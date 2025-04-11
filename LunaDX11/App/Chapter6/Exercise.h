#pragma once

#include "Core/SphericalCamera.h"
#include "Rendering/Submesh.h"

#include <array>

using namespace DirectX;

class Exercise : public SphericalCamera
{
    DECLARE_ENGINE(Exercise, SphericalCamera)

public:
    ~Exercise() override = default;

    bool Init(HINSTANCE inInstanceHandle) override;
    LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
    void OnResize() override;

protected:
    Exercise();

    void Update(float deltaSeconds) override;
    void Render() override;

private:
    void InitShader();
    void CreateGeometry();

    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;

    ComPtr<ID3D11InputLayout> inputLayout;

    ComPtr<ID3D11Buffer> wvpMatrixBuffer;

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    XMFLOAT4X4 viewMatrix;
    XMFLOAT4X4 projectionMatrix;

    std::array<XMFLOAT3, 8> pointPositions{};

    XMFLOAT4X4 boxWorldMatrix{};
    Submesh boxSubmesh{};

    XMFLOAT4X4 pyramidWorldMatrix{};
    Submesh pyramidSubmesh{};
};
