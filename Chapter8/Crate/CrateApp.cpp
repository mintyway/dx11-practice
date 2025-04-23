#include "CrateApp.h"

#include <numbers>
#include <External/DirectXTex/DirectXTex.h>

#include "Core/Common/GeometryGenerator.h"
#include "Core/Common/Timer.h"
#include "Core/Data/Color.h"
#include "Core/Data/Path.h"
#include "Core/Rendering/Vertex.h"
#include "Shaders/CrateShaderPass.h"

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
    : DirectionalLights(
          {
              DirectionalLight
              {
                  .ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),
                  .diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
                  .specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
                  .direction = XMFLOAT3(std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>)
              },
              DirectionalLight
              {
                  .ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
                  .diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f),
                  .specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f),
                  .direction = XMFLOAT3(-std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>)
              },
              DirectionalLight
              {
                  .ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
                  .diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),
                  .specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
                  .direction = XMFLOAT3(0.0f, 1.0f / -std::numbers::sqrt2_v<float>, 1.0f / -std::numbers::sqrt2_v<float>)
              }
          }
      ),
      material(
          {
              .ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
              .diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
              .specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f)
          }
      )
{
    SetCameraSphericalCoord(3.0f, -XM_PIDIV4, XM_PIDIV4);
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&TextureTransformMatrix, XMMatrixIdentity());
}

CrateApp::~CrateApp() = default;

bool CrateApp::Init(HINSTANCE inInstanceHandle)
{
    if (!SphericalCamera::Init(inInstanceHandle))
    {
        return false;
    }

    shaderPass = std::make_unique<CrateShaderPass>(device.Get());
    shaderPass->Bind(immediateContext.Get());

    InitGeometry();
    InitTexture();

    return true;
}

void CrateApp::OnResize()
{
    SphericalCamera::OnResize();

    XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(XM_PIDIV4, GetAspectRatio(), 0.1f, 1000.0f));
}

void CrateApp::Update(float deltaSeconds)
{
    const XMVECTOR cameraPosition = SphericalCoord::SphericalToCartesian(GetCameraSphericalCoord());
    const XMVECTOR focusPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(cameraPosition, focusPosition, upDirection));
    XMStoreFloat3(&eyePosition, cameraPosition);

    if (GetAsyncKeyState('0') & 0x8000)
    {
        activeLightCount = 0;
    }
    if (GetAsyncKeyState('1') & 0x8000)
    {
        activeLightCount = 1;
    }
    if (GetAsyncKeyState('2') & 0x8000)
    {
        activeLightCount = 2;
    }
    if (GetAsyncKeyState('3') & 0x8000)
    {
        activeLightCount = 3;
    }
}

void CrateApp::Render()
{
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), LinearColors::Silver);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    shaderPass->SetMatrix(XMLoadFloat4x4(&WorldMatrix), XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projectionMatrix));
    shaderPass->SetMaterial(material);
    shaderPass->SetTextureTransformMatrix(TextureTransformMatrix);
    shaderPass->SetUseTexture(true);
    shaderPass->SetEyePosition(eyePosition);
    shaderPass->SetDirectionalLights(DirectionalLights);
    shaderPass->SetActiveDirectionalLightCount(activeLightCount);
    shaderPass->UpdateCBuffer(immediateContext.Get());
    immediateContext->DrawIndexed(indexCount, 0, 0);

    swapChain->Present(0, 0);
}

void CrateApp::InitGeometry()
{
    const auto [boxVertices, boxIndices] = GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f);

    std::vector<Vertex::PNT> vertices(boxVertices.size());
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        vertices[i].position = boxVertices[i].position;
        vertices[i].normal = boxVertices[i].normal;
        vertices[i].tex = boxVertices[i].texC;
    }

    std::vector<UINT> indices;
    indices.insert(indices.end(), boxIndices.begin(), boxIndices.end());
    indexCount = static_cast<UINT>(indices.size());

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex::PNT) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{.pSysMem = vertices.data()};
    device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &vertexBuffer);
    immediateContext->IASetVertexBuffers(
        0, 1,
        std::array{vertexBuffer.Get()}.data(),
        std::array{static_cast<UINT>(sizeof(Vertex::PNT))}.data(), std::array{0u}.data()
    );

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * indices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{.pSysMem = indices.data()};
    device->CreateBuffer(&indexBufferDesc, &indexInitData, &indexBuffer);
    immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CrateApp::InitTexture()
{
    ScratchImage image;
    TexMetadata metadata;
    LoadFromDDSFile(Path::GetTexturePath(L"WoodCrate01.dds").c_str(), DDS_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(device.Get(), image.GetImages(), image.GetImageCount(), metadata, &diffuseMapSRV);
    shaderPass->SetDiffuseMap(immediateContext.Get(), diffuseMapSRV.Get());
}
