#include <Windows.h>

#include "Core/Engine.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    // 게임 엔진 초기화
    if (!Engine::Get()->Init(hInstance, [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return Engine::Get()->HandleMessage(hWnd, message, wParam, lParam); }))
    {
        return 0;
    }

    // 엔진 실행
    Engine::Get()->Run();

    return 0;
}
