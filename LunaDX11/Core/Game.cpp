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
    const HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &device, &featureLevel, &immediateContext);

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

    return true;
}

void Game::Update() {}

void Game::Render() {}

void Game::Release() {}
