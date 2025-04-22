#include "CrateApp.h"

using namespace DirectX;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    const std::unique_ptr<EngineBase> engine = std::make_unique<CrateApp>();
    if (engine->Init(hInstance))
    {
        engine->Run();
    }

    return 0;
}

CrateApp::CrateApp()
{
    
}

CrateApp::~CrateApp()
{
    
}

bool CrateApp::Init(HINSTANCE inInstanceHandle)
{
    return SphericalCamera::Init(inInstanceHandle);
}

void CrateApp::OnResize()
{
    SphericalCamera::OnResize();
}

void CrateApp::Update(float deltaSeconds) {}

void CrateApp::Render() {}
