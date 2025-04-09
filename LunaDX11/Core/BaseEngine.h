#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>

#include "GameInstance.h"
#include "Timer.h"

using namespace Microsoft::WRL;

#define DECLARE_ENGINE(ClassName, ParentClassName)\
    friend class BaseEngine;\
    using Super = ParentClassName;\
    using ThisClass = ClassName;\
    public:\
    ClassName(const ClassName&) = delete;\
    ClassName& operator=(const ClassName&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
    private:

enum class WindowState : uint8_t
{
    Window,
    Minimized,
    Maximized
};

class BaseEngine
{
public:
    virtual ~BaseEngine() = default;
    BaseEngine(const BaseEngine&) = delete;
    BaseEngine& operator=(const BaseEngine&) = delete;

    template <typename T, typename = std::enable_if_t<std::is_base_of_v<BaseEngine, T>>>
    static void Register()
    {
        if (!engineInstance) // 등록된 이후 바꿀 수 없음
        {
            engineInstance.reset(new T);
        }
    }

    static BaseEngine* Get() { return engineInstance.get(); }

    template <typename T>
    static T* Get() { return dynamic_cast<T*>(Get()); }

    float GetAspectRatio() const { return static_cast<float>(clientWidth) / static_cast<float>(clientHeight); }

    virtual bool Init(HINSTANCE inInstanceHandle);

    void Run();
    virtual LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void Tick();

    virtual void OnResize();

    virtual void OnMouseDown(WPARAM buttonState, int x, int y) = 0;
    virtual void OnMouseUp(WPARAM buttonState, int x, int y) = 0;
    virtual void OnMouseMove(WPARAM buttonState, int x, int y) = 0;

protected:
    BaseEngine() = default;

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
    WindowState windowState;
    bool isResizing = false;

    int clientWidth = 800;
    int clientHeight = 600;

    Timer timer;
    std::wostringstream frameInfoStream = []
    {
        std::wostringstream oss;
        oss << std::fixed << std::setprecision(6);
        return oss;
    }();

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> immediateContext;
    ComPtr<IDXGISwapChain> swapChain;
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<ID3D11DepthStencilView> depthStencilView;
    ComPtr<ID3D11RasterizerState> wireframeRasterizerState;
    UINT msaaQuality = 0;

    std::unique_ptr<GameInstance> gameInstance;

private:
    static std::unique_ptr<BaseEngine> engineInstance;
};
