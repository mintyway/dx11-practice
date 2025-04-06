#include "BaseEngine.h"

#include "Utility.h"

#include <windowsx.h>
#include <thread>

namespace
{
    LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return BaseEngine::Get()->HandleMessage(hWnd, message, wParam, lParam); }
}

std::unique_ptr<BaseEngine> BaseEngine::engineInstance;

bool BaseEngine::Init(HINSTANCE inInstanceHandle)
{
    instanceHandle = inInstanceHandle;
    className = L"Engine";
    windowName = L"DirectX11";

    if (!InitWindow())
    {
        return false;
    }

    if (!InitDirectX())
    {
        return false;
    }

    gameInstance = std::make_unique<GameInstance>();

    return true;
}

void BaseEngine::Run()
{
    timer.Start();

    MSG message{};
    while (message.message != WM_QUIT)
    {
        while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        Tick();
    }
}

LRESULT BaseEngine::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_ACTIVATE:
        {
            if (LOWORD(wParam) == WA_INACTIVE)
            {
                isPaused = true;
                timer.Stop();
            }
            else
            {
                isPaused = false;
                timer.Start();
            }

            return 0;
        }
        case WM_SIZE:
        {
            clientWidth = LOWORD(lParam);
            clientHeight = HIWORD(lParam);

            switch (wParam)
            {
                case SIZE_MINIMIZED:
                {
                    OutputDebugString(L"최소화\n");
                    isPaused = true;
                    windowState = WindowState::Minimized;
                    break;
                }
                case SIZE_MAXIMIZED:
                {
                    OutputDebugString(L"최대화\n");
                    isPaused = false;
                    windowState = WindowState::Maximized;
                    OnResize();
                    break;
                }
                case SIZE_RESTORED:
                {
                    if (windowState == WindowState::Minimized || windowState == WindowState::Maximized)
                    {
                        OutputDebugString(L"창모드\n");
                        isPaused = false;
                        windowState = WindowState::Window;
                        OnResize();
                    }
                    else if (isResizing) {}
                    else
                    {
                        OutputDebugString(L"전체화면\n");
                        OnResize();
                    }

                    break;
                }
                default:
                    break;
            }

            return 0;
        }
        case WM_ENTERSIZEMOVE:
        {
            isPaused = true;
            isResizing = true;
            timer.Stop();
            return 0;
        }
        case WM_EXITSIZEMOVE:
        {
            isPaused = false;
            isResizing = false;
            timer.Start();
            OnResize();
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_MENUCHAR: // Alt + Enter 전체화면 비프음 제거
        {
            return MAKELRESULT(0, MNC_CLOSE);
        }
        case WM_GETMINMAXINFO: // 최소 앱 크기 제한
        {
            MINMAXINFO* minMax = reinterpret_cast<MINMAXINFO*>(lParam); // NOLINT(performance-no-int-to-ptr)
            minMax->ptMinTrackSize.x = 200;
            minMax->ptMinTrackSize.y = 200;
            return 0;
        }
        case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE)
            {
                OutputDebugString(L"ESC");
                PostQuitMessage(0);
            }

            return 0;
        }
        case WM_LBUTTONDOWN:
            [[fallthrough]];
        case WM_MBUTTONDOWN:
            [[fallthrough]];
        case WM_RBUTTONDOWN:
        {
            OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        }
        case WM_LBUTTONUP:
            [[fallthrough]];
        case WM_MBUTTONUP:
            [[fallthrough]];
        case WM_RBUTTONUP:
        {
            OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        }
        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        };
    }
}

void BaseEngine::Tick()
{
    timer.Tick();

    if (!isPaused)
    {
        const float deltaSeconds = timer.GetDeltaSeconds();
        UpdateFrameInfo(deltaSeconds);
        Update(deltaSeconds);
        Render();
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void BaseEngine::OnResize()
{
    if (!device || !immediateContext || !swapChain) // dx가 초기화 되기 전이라면 아무것도 수행하지 않습니다.
    {
        return;
    }

    // 이전 렌더 타겟 바인등 해제 및 이전 뷰, 버퍼 해제
    // 이전 프레임의 렌더 타겟 혹은 버퍼가 GPU에서 사용중이면 ResizeBuffer에 실패하는데, 이를 방지하고자 깔끔하게 초기화해줌.
    immediateContext->OMSetRenderTargets(0, nullptr, nullptr);
    renderTargetView.Reset();
    depthStencilView.Reset();

    HRESULT hr = swapChain->ResizeBuffers(1, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    CHECK_HR(hr, L"백버퍼 사이즈 변경 실패");

    // 백버퍼 텍스처 가져오기
    ComPtr<ID3D11Texture2D> backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
    CHECK_HR(hr, L"백버퍼 가져오기 실패");

    // 렌더 타겟 뷰 생성
    hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
    CHECK_HR(hr, L"렌더 타겟 뷰 생성 실패");

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = clientWidth;
    depthStencilDesc.Height = clientHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 4;
    depthStencilDesc.SampleDesc.Quality = msaaQuality - 1;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> depthStencilBuffer;
    hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
    CHECK_HR(hr, L"깊이/스텐실 텍스쳐 생성 실패");

    // 깊이/스텐실 뷰 생성
    hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, &depthStencilView);
    CHECK_HR(hr, L"깊이/스텐실 뷰 생성 실패");

    // 출력 병합기 바인딩
    immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<FLOAT>(clientWidth);
    viewport.Height = static_cast<FLOAT>(clientHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // 뷰포트 설정
    immediateContext->RSSetViewports(1, &viewport);
}

bool BaseEngine::InitWindow()
{
    // 윈도우 클래스 정의 및 등록
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instanceHandle;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = className.c_str();
    if (!RegisterClassEx(&wc))
    {
        MessageBox(nullptr, L"클래스 등록 실패", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // 창의 UI를 포함하지 않은 클라이언트 영역의 사이즈가 clientWidth, clientHeight가 되도록 보정
    RECT rect = {0, 0, clientWidth, clientHeight};
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_APPWINDOW);
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;

    // 윈도우를 스크린 중심에 띄우기 위한 오프셋
    const int centerX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    const int centerY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    // 윈도우 생성
    windowHandle = CreateWindowEx(WS_EX_APPWINDOW, className.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW, centerX, centerY, width, height, nullptr, nullptr, instanceHandle, nullptr);
    if (!windowHandle)
    {
        MessageBox(nullptr, L"윈도우 생성 실패", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // 윈도우 표시
    ShowWindow(windowHandle, SW_SHOW);
    UpdateWindow(windowHandle);

    return true;
}

bool BaseEngine::InitDirectX()
{
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    /** 매개변수 정리
     * pAdapter: 디스플레이 어댑터. nullptr 지정 시 기본 디스플레이 어댑터가 사용됨.
     * DriverType: 일반적으로 Hardware로 설정하여 그래픽카드가 렌더링 하도록함. reference는 cpu를 사용하며, 이는 그래픽카드가 지원하지 않는 기능을 테스트하기 위해 존재한다. Software도 CPU이다.
     * Software: DriverType을 소프트웨어로 사용시 소프트웨어 구동기를 지정한다. 하드웨어 사용시는 nullptr 지정.
     * Flags: 디바이스 생성 옵션들로 디버그를 활성화하거나 멀티스레드를 사용하지 않는 경우 꺼 싱글스레드 성능을 향상시킬 수 있다.
     * pFeatureLevels: D3D_FEATURE_LEVEL 형식 원소들의 배열로 순서대로 점검하며 가장 높게 지원되는 dx 기능을 지정한다. nullptr 지정시 가장 높은 버전으로 선택됨.
     * FeatureLevels: 위 배열의 크기이다. nullptr시 0으로 지정.
     * SDKVersion: 현재는 다이렉트x11로 D3D11 SDK를 사용하면된다.
     * ppDevice: 생성된 Device를 반환한다.
     * pFeatureLevel: pFeatureLevels 배열에서 처음으로 지원되는 기능을 반환한다. 어떤 버전이 선택되었는지 확인용.
     * ppImmediateContext: 생성된 immediate device context를 반환한다.
     */
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &device, &featureLevel, &immediateContext);
    CHECK_HR(hr, L"D3D11 Device 생성 실패", false);

    if (featureLevel < D3D_FEATURE_LEVEL_11_0)
    {
        MessageBox(nullptr, L"dx11 미지원", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    /** 매개변수 정리
     * Format: 여기서는 백버퍼 포맷
     * SampleCount: 샘플링 개수로 4는 4x MSAA를 말함.
     * pNumQualityLevels: 가능한 품질 수준 반환 (1 이상이어야 사용 가능)
     * msaaQuality: 품질이 0이면 4x msaa 미지원
     */
    hr = device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality);
    CHECK_HR(hr, L"4x MSAA 품질 검사 실패", false);

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = clientWidth; // 백 버퍼 너비
    sd.BufferDesc.Height = clientHeight; // 백 버퍼 높이
    sd.BufferDesc.RefreshRate.Numerator = 60; // 리프레시율 (60Hz)
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32비트 RGBA 포맷
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 화면을 그리는 순서를 지정하는 것으로 해당 옵션은 드라이버가 알아서 설정하는 방식이다.
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 해상도가 불일치 할경우 스케일링을 어떻게 할지 지정하는 것으로 해당 옵션은 드라이버가 알아서 설정하는 방식이다.
    sd.SampleDesc.Count = 4; // msaa 샘플링 개수 (1이면 비활성화)
    sd.SampleDesc.Quality = msaaQuality - 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 백버퍼 용도
    sd.BufferCount = 1; // 추가 버퍼 하나를 사용해서 더블 버퍼로 사용하겠다는 의미
    sd.OutputWindow = windowHandle; // 출력할 윈도우
    sd.Windowed = true; // 창모드
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 가장 일반적인 스왑 효과 전환하고나면 버퍼의 모든 내용을 버린다.
    sd.Flags = 0;

    // 스왑체인은 반드시 디바이스를 생성한 팩토리를 통해서만 만들 수 있음. 아래는 이를 얻기 위한 과정.
    ComPtr<IDXGIDevice> dxgiDevice;
    device->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice); // dxgi 계층 구조로 접근하기위해 IDXGIDevice를 얻어옴

    ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(&dxgiAdapter); // 실제로 디바이스가 할당 받은 어댑터(그래픽카드)를 얻음

    ComPtr<IDXGIFactory> dxgiFactory;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory), &dxgiFactory); // 어댑터의 부모만이 스왑체인을 생성할 수 있는 인터페이스임.

    hr = dxgiFactory->CreateSwapChain(device.Get(), &sd, &swapChain); // 스왑체인 생성.
    CHECK_HR(hr, L"스왑체인 생성 실패", false);

    OnResize();

    return true;
}

void BaseEngine::UpdateFrameInfo(float deltaSeconds)
{
    static int frameCount = 0;
    static float elpasedTime = 0.0f;

    ++frameCount;
    elpasedTime += deltaSeconds;

    if (elpasedTime >= 1.0f)
    {
        const float msPerFrame = 1000.0f / static_cast<float>(frameCount);

        frameInfoStream.str(L"");
        frameInfoStream.clear();
        frameInfoStream << windowName.c_str() << L" - FPS: " << frameCount << L"\tFrame Time: " << msPerFrame << L"ms";
        SetWindowText(windowHandle, frameInfoStream.str().c_str());

        frameCount = 0;
        elpasedTime = 0.0f;
    }
}

void BaseEngine::Update(float deltaSeconds) {}

void BaseEngine::Render() {}
