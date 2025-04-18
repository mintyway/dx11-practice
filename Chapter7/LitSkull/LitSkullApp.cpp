#include "LitSkullApp.h"

#include "Core/Data/Path.h"

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
    : DirectionalLights({
          DirectionalLight{
              .ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),
              .diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
              .specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
              .direction = XMFLOAT3(std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>)
          },
          DirectionalLight{
              .ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
              .diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f),
              .specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f),
              .direction = XMFLOAT3(-std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>)
          },
          DirectionalLight{
              .ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
              .diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),
              .specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
              .direction = XMFLOAT3(0.0f, 1.0f / -std::numbers::sqrt2_v<float>, 1.0f / -std::numbers::sqrt2_v<float>)
          }
      }),
      MaterialMap({
          {"Grid", Material{
              .ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f),
              .diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f),
              .specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f)
          }},
          {"Cylinder", Material{
              .ambient = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f),
              .diffuse = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f),
              .specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f)
          }},
          {"Sphere", Material{
              .ambient = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f),
              .diffuse = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f),
              .specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f)
          }},
          {"Box", Material{
              .ambient = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f),
              .diffuse = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f),
              .specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f)
          }},
          {"Skull", Material{
              .ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
              .diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
              .specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f)
          }}
      })
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
    if (!SphericalCamera::Init(inInstanceHandle))
    {
        return false;
    }

    return true;
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

void LitSkullApp::InitShaderResource()
{
    ComPtr<ID3DBlob> vertexShaderBlob;
    D3DReadFileToBlob(Path::GetShaderPath(L"Light_vs.cso").c_str(), &vertexShaderBlob);
    device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader);

    ComPtr<ID3DBlob> pixelShaderBlob;
    D3DReadFileToBlob(Path::GetShaderPath(L"Light_ps.cso").c_str(), &pixelShaderBlob);
    device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader);
    
    device->CreateInputLayout()
}

void LitSkullApp::InitGeometryBuffer()
{
    InitSkullBuffer();
    InitShapeBuffer();
}

void LitSkullApp::InitSkullBuffer() {}

void LitSkullApp::InitShapeBuffer() {}
