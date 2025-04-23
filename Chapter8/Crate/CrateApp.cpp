#include "CrateApp.h"
#include <External/DirectXTex/DirectXTex.h>

#include "Core/Data/Path.h"

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

CrateApp::CrateApp() {}

CrateApp::~CrateApp() {}

bool CrateApp::Init(HINSTANCE inInstanceHandle)
{
    if (!SphericalCamera::Init(inInstanceHandle))
    {
        return false;
    }

    ScratchImage image;
    TexMetadata metadata;
    LoadFromDDSFile(Path::GetTexturePath(L"WoodCrate01.dds").c_str(), DDS_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(device.Get(), image.GetImages(), image.GetImageCount(), metadata, &diffuseMapSRV);

    return true;
}

void CrateApp::OnResize()
{
    SphericalCamera::OnResize();
}

void CrateApp::Update(float deltaSeconds) {}

void CrateApp::Render() {}
