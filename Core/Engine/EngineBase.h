#pragma once

#include <d3d11.h>
#include <iomanip>
#include <memory>
#include <sstream>
#include <Windows.h>
#include <wrl/client.h>

enum class WindowState : uint8_t
{
    Window,
    Minimized,
    Maximized
};

#define DECLARE_ENGINE(ClassName, ParentClassName)\
using Super = ParentClassName;\
using ThisClass = ClassName;\
public:\
ClassName(const ClassName&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
ClassName& operator=(const ClassName&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
ClassName(ClassName&&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
ClassName& operator=(ClassName&&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
private:

class Timer;

class EngineBase
{
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    EngineBase();
    virtual ~EngineBase();

    EngineBase(const EngineBase&) = delete;
    EngineBase& operator=(const EngineBase&) = delete;
    EngineBase(EngineBase&&) noexcept = delete;
    EngineBase& operator=(EngineBase&&) noexcept = delete;

    [[nodiscard]] float GetAspectRatio() const { return static_cast<float>(clientWidth) / static_cast<float>(clientHeight); }

    virtual bool Init(HINSTANCE inInstanceHandle);

    void Run();
    virtual LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void Tick();

    virtual void OnResize();

    virtual void OnMouseDown(WPARAM buttonState, int x, int y) = 0;
    virtual void OnMouseUp(WPARAM buttonState, int x, int y) = 0;
    virtual void OnMouseMove(WPARAM buttonState, int x, int y) = 0;

protected:
    bool InitWindow();
    bool InitDirectX();

    void UpdateFrameInfo(float deltaSeconds);

    virtual void Update(float deltaSeconds) = 0;
    virtual void Render() = 0;

    bool IsWireframe() { return bUseWireframeView; }

    HINSTANCE instanceHandle = nullptr;
    HWND windowHandle = nullptr;
    std::wstring className;
    std::wstring windowName;
    bool isPaused = false;
    WindowState windowState{};
    bool isResizing = false;

    int clientWidth = 800;
    int clientHeight = 600;

    std::unique_ptr<Timer> timer;
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
    UINT msaaQuality = 0;

    ComPtr<ID3D11RasterizerState> wireframeRasterizerState;
    ComPtr<ID3D11RasterizerState> noCullRasterizerState;
    ComPtr<ID3D11RasterizerState> counterClockwiseRasterizerState;

    ComPtr<ID3D11BlendState> alphaToCoverageBlendState;
    ComPtr<ID3D11BlendState> transparentBlendState;
    ComPtr<ID3D11BlendState> noRenderTargetWriteBlendState;

    ComPtr<ID3D11DepthStencilState> markMirrorDepthStencilState;
    ComPtr<ID3D11DepthStencilState> renderReflectionDepthStencilState;
    ComPtr<ID3D11DepthStencilState> noDoubleBlendDepthStencilState;

private:
    static std::unique_ptr<EngineBase> engineInstance;
    bool bUseWireframeView = false;
};
