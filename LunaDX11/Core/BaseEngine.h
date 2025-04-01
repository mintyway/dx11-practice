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
    virtual ~BaseEngine();
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

    virtual bool Init(HINSTANCE inInstanceHandle);

    void Run();
    virtual LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void Tick();

    virtual void OnResize();

    virtual void OnMouseDown(WPARAM buttonState, int x, int y) {}
    virtual void OnMouseUp(WPARAM buttonState, int x, int y) {}
    virtual void OnMouseMove(WPARAM buttonState, int x, int y) {}

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

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* immediateContext = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11DepthStencilView* depthStencilView = nullptr;
    UINT msaaQuality = 0;

    std::unique_ptr<GameInstance> gameInstance;

private:
    static std::unique_ptr<BaseEngine> engineInstance;
};
