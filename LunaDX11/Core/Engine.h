#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <memory>
#include <iomanip>
#include <sstream>

#include "GameInstance.h"
#include "Timer.h"

class Engine
{
public:
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    static Engine* Get()
    {
        static Engine instance;
        return &instance;
    }

    virtual bool Init(HINSTANCE hInstance);

    void Run();
    virtual LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void Tick();

    virtual void OnResize();

    virtual void OnMouseDown(WPARAM buttonState, int x, int y) {}
    virtual void OnMouseUp(WPARAM buttonState, int x, int y) {}
    virtual void OnMouseMove(WPARAM buttonState, int x, int y) {}

private:
    Engine() = default;
    virtual ~Engine();

    bool InitWindow();
    bool InitDirectX();

    void UpdateFrameInfo(float deltaSeconds);

    virtual void Update(float deltaSeconds);
    virtual void Render();

    HINSTANCE instanceHandle = nullptr;
    HWND windowHandle = nullptr;
    std::wstring className;
    std::wstring windowName;
    bool isPaused = false;

    int clientWidth = 800;
    int clientHeight = 600;

    Timer timer;
    std::wostringstream frameInfoStream = []
    {
        std::wostringstream oss;
        oss << std::fixed << std::setprecision(6);
        return oss;
    }();

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* immediateContext = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11DepthStencilView* depthStencilView = nullptr;
    UINT msaaQuality;

    std::unique_ptr<GameInstance> gameInstance;
};
