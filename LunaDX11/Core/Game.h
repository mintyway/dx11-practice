#pragma once

#include <Windows.h>

class Game
{
public:
    bool Init(HWND hWnd);
    void Update();
    void Render();
    void Release();
};
