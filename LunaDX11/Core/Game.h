#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

class Game
{
public:
    bool Init(HWND hWnd);
    void Update();
    void Render();
    void Release();

private:
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* immediateContext = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    D3D_FEATURE_LEVEL featureLevel{};
};
