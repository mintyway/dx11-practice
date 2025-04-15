#include "LightingApp.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    BaseEngine::Register<LightingApp>();

    if (!BaseEngine::Get()->Init(hInstance))
    {
        return 0;
    }

    BaseEngine::Get()->Run();

    return 0;
}

bool LightingApp::Init(HINSTANCE inInstanceHandle)
{
    if (!SphericalCamera::Init(inInstanceHandle))
    {
        return false;
    }

    return true;
}

void LightingApp::OnResize()
{
    SphericalCamera::OnResize();
}

void LightingApp::Update(float deltaSeconds) {}

void LightingApp::Render() {}
