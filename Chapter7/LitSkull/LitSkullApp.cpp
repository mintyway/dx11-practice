#include "LitSkullApp.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    BaseEngine::Register<LitSkullApp>();

    if (!BaseEngine::Get()->Init(hInstance))
    {
        return 0;
    }

    BaseEngine::Get()->Run();

    return 0;
}

LitSkullApp::LitSkullApp()
{
    // SetCameraSphericalCoord();

    XMStoreFloat4x4(&skullWorldMatrix, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.0f, 1.0f, 0.0f));
    XMStoreFloat4x4(&boxWorldMatrix, XMMatrixScaling(3.0f, 1.0f, 3.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
    XMStoreFloat4x4(&gridWorldMatrix, XMMatrixIdentity());

    for (int i = 0; i < 5; ++i)
    {
        const int baseIndex = i * 2;
        constexpr float PositionX = 5.0f;
        constexpr float baseZ = -10.0f;
        const float offsetZ = static_cast<float>(i) * 5.0f;

        constexpr float sphereHeight = 3.5f;
        XMStoreFloat4x4(&sphereWorldMatrices[baseIndex], XMMatrixTranslation(-PositionX, sphereHeight, baseZ + offsetZ));
        XMStoreFloat4x4(&sphereWorldMatrices[baseIndex + 1], XMMatrixTranslation(PositionX, sphereHeight, baseZ + offsetZ));

        constexpr float cylinderHeight = 1.5f;
        XMStoreFloat4x4(&cylinderMatrices[baseIndex], XMMatrixTranslation(-PositionX, cylinderHeight, baseZ + offsetZ));
        XMStoreFloat4x4(&cylinderMatrices[baseIndex + 1], XMMatrixTranslation(PositionX, cylinderHeight, baseZ + offsetZ));
    }
}

bool LitSkullApp::Init(HINSTANCE inInstanceHandle)
{
    return SphericalCamera::Init(inInstanceHandle);
}

LRESULT LitSkullApp::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return SphericalCamera::HandleMessage(hWnd, message, wParam, lParam);
}

void LitSkullApp::OnResize()
{
    SphericalCamera::OnResize();
}

void LitSkullApp::Update(float deltaSeconds)
{
    SphericalCamera::Update(deltaSeconds);
}

void LitSkullApp::Render()
{
    SphericalCamera::Render();
}
