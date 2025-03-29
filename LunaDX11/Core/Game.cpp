#include "Game.h"

bool Game::Init(HWND hWnd)
{
    UINT createDeviceFlags = 0;

#if defined(DEBUG)
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
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &device, &featureLevel, &immediateContext);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"D3D11 Device 생성 실패", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
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
    UINT msaaQuality;
    hr = device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"4x MSAA 품질 검사 실패", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    RECT rc;
    GetClientRect(hWnd, &rc);

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferDesc.Width = rc.right - rc.left; // 백 버퍼 너비
    sd.BufferDesc.Height = rc.bottom - rc.top; // 백 버퍼 높이
    sd.BufferDesc.RefreshRate.Numerator = 60; // 리프레시율 (60Hz)
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32비트 RGBA 포맷
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 화면을 그리는 순서를 지정하는 것으로 해당 옵션은 드라이버가 알아서 설정하는 방식이다.
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 해상도가 불일치 할경우 스케일링을 어떻게 할지 지정하는 것으로 해당 옵션은 드라이버가 알아서 설정하는 방식이다.
    sd.SampleDesc.Count = 4; // msaa 샘플링 개수 (1이면 비활성화)
    sd.SampleDesc.Quality = msaaQuality - 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 백버퍼 용도
    sd.BufferCount = 1; // 추가 버퍼 하나를 사용해서 더블 버퍼로 사용하겠다는 의미
    sd.OutputWindow = hWnd; // 출력할 윈도우
    sd.Windowed = true; // 창모드
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 가장 일반적인 스왑 효과 전환하고나면 버퍼의 모든 내용을 버린다.
    sd.Flags = 0;

    IDXGIFactory* dxgiFactory = nullptr; // 스왑체인은 반드시 디바이스를 생성한 팩토리를 통해서만 만들 수 있음. 이를 얻기 위한 과정.
    {
        IDXGIDevice* dxgiDevice;
        device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)); // dxgi 계층 구조로 접근하기위해 IDXGIDevice를 얻어옴

        IDXGIAdapter* dxgiAdapter;
        dxgiDevice->GetAdapter(&dxgiAdapter); // 실제로 디바이스가 할당 받은 어댑터(그래픽카드)를 얻음

        dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory)); // 어댑터의 부모는 스왑체인을 생성할 수 있는 인터페이스임.

        // COM 인터페이스는 가져오는 순간 AddRef()되며 참조 카운트가 증가하기때문에 쓰고 후 반드시 Release로 참조해제해야함.
        dxgiDevice->Release();
        dxgiAdapter->Release();
    }

    hr = dxgiFactory->CreateSwapChain(device, &sd, &swapChain); // 스왑체인 생성.
    dxgiFactory->Release();
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"스왑체인 생성 실패", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // 백버퍼 텍스처 가져오기
    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"백버퍼 가져오기 실패", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // 렌더 타겟 뷰 생성
    hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"렌더 타겟 뷰 생성 실패", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

void Game::Update() {}

void Game::Render() {}

void Game::Release() {}
