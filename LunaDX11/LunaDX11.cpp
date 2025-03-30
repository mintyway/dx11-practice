#include <Windows.h>

#include "Core/Engine.h"
#include <iostream>

void CreateDebugConsole()
{
    FILE* dummy; // 매개변수 채우기 용 더미 포인터
    AllocConsole(); // 콘솔 할당
    (void)freopen_s(&dummy, "CONOUT$", "w", stdout); // 표준 출력 재지정
    (void)freopen_s(&dummy, "CONOUT$", "w", stderr); // 표준 에러 재지정
    (void)freopen_s(&dummy, "CONIN$", "r", stdin); // 표준 입력 재지정
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    CreateDebugConsole();

    // 게임 엔진 초기화
    if (!Engine::Get()->Init(hInstance, [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return Engine::Get()->HandleMessage(hWnd, message, wParam, lParam); }))
    {
        return 0;
    }

    // 엔진 실행
    Engine::Get()->Run();

    return 0;
}
