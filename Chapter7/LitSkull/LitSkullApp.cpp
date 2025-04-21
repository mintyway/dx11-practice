#include "LitSkullApp.h"

#include <bitset>
#include <fstream>
#include <numbers>

#include "Core/Common/GeometryGenerator.h"
#include "Core/Data/Color.h"
#include "Core/Data/Path.h"
#include "Core/Rendering/Vertex.h"
#include "Shaders/ShaderPass.h"

using namespace DirectX;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    const std::unique_ptr<EngineBase> engine = std::make_unique<LitSkullApp>();
    if (engine->Init(hInstance))
    {
        engine->Run();
    }

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
      RenderAssetMap({
          {ObjectType::Grid, {
              .material = {
                  .ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f),
                  .diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f),
                  .specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f)}}},
          {ObjectType::Cylinder, {
              .material = {
                  .ambient = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f),
                  .diffuse = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f),
                  .specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f)}}},
          {ObjectType::Sphere, {
              .material = {
                  .ambient = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f),
                  .diffuse = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f),
                  .specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f)}}},
          {ObjectType::Box, {
              .material = {
                  .ambient = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f),
                  .diffuse = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f),
                  .specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f)}}},
          {ObjectType::Skull, {
              .material = {
                  .ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
                  .diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
                  .specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f)}}}
      })
{
    SetCameraSphericalCoord(10.0f, -XM_PIDIV4, XM_PIDIV4);

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
    if (!Super::Init(inInstanceHandle))
    {
        return false;
    }

    InitShaderPass();
    InitGeometryBuffer();

    return true;
}

LRESULT LitSkullApp::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return Super::HandleMessage(hWnd, message, wParam, lParam);
}

void LitSkullApp::OnResize()
{
    Super::OnResize();

    XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(XM_PIDIV4, GetAspectRatio(), 0.1f, 1000.0f));
}

void LitSkullApp::Update(float deltaSeconds)
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

void LitSkullApp::Render()
{
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), LinearColors::Silver);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    RenderObject(XMLoadFloat4x4(&skullWorldMatrix), RenderAssetMap[ObjectType::Skull].material, RenderAssetMap[ObjectType::Skull].submesh);
    RenderObject(XMLoadFloat4x4(&boxWorldMatrix), RenderAssetMap[ObjectType::Box].material, RenderAssetMap[ObjectType::Box].submesh);
    RenderObject(XMLoadFloat4x4(&gridWorldMatrix), RenderAssetMap[ObjectType::Grid].material, RenderAssetMap[ObjectType::Grid].submesh);
    
    for (size_t i = 0; i < sphereWorldMatrices.size(); ++i)
    {
        RenderObject(XMLoadFloat4x4(&sphereWorldMatrices[i]), RenderAssetMap[ObjectType::Sphere].material, RenderAssetMap[ObjectType::Sphere].submesh);
    }
    
    for (size_t i = 0; i < cylinderMatrices.size(); ++i)
    {
        RenderObject(XMLoadFloat4x4(&cylinderMatrices[i]), RenderAssetMap[ObjectType::Cylinder].material, RenderAssetMap[ObjectType::Cylinder].submesh);
    }

    swapChain->Present(0, 0);
}

void LitSkullApp::RenderObject(FXMMATRIX worldMatrix, const Material& material, const Submesh& submesh)
{
    shaderPass->SetMatrix(worldMatrix, XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projectionMatrix));
    shaderPass->SetMaterial(material);
    shaderPass->SetEyePosition(eyePosition);
    shaderPass->SetDirectionalLights(DirectionalLights);
    shaderPass->SetActiveDirectionalLightCount(activeLightCount);
    shaderPass->UpdateCBuffer(immediateContext.Get());
    immediateContext->DrawIndexed(submesh.indexCount, submesh.startIndexLocation, submesh.baseVertexLocation);
}

void LitSkullApp::InitShaderPass()
{
    shaderPass = std::make_unique<ShaderPass>(device.Get());
    shaderPass->Bind(immediateContext.Get());
}

void LitSkullApp::InitGeometryBuffer()
{
    std::vector<Vertex::PN> vertices;
    std::vector<UINT> indices;

    InitSkullBuffer(vertices, indices);
    InitShapeBuffer(vertices, indices);

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex::PN) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{.pSysMem = vertices.data()};
    device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &vertexBuffer);
    immediateContext->IASetVertexBuffers(0, 1, std::array{vertexBuffer.Get()}.data(), std::array{static_cast<UINT>(sizeof(Vertex::PN))}.data(), std::array{0u}.data());

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * indices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{.pSysMem = indices.data()};
    device->CreateBuffer(&indexBufferDesc, &indexInitData, &indexBuffer);
    immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void LitSkullApp::InitSkullBuffer(std::vector<Vertex::PN>& inoutVertices, std::vector<UINT>& inoutIndices)
{
    std::ifstream ifs(Path::GetModelPath(L"skull.txt").c_str());

    UINT vertexCount = 0;
    UINT triangleCount = 0;
    std::string ignore;

    ifs >> ignore >> vertexCount;
    ifs >> ignore >> triangleCount;
    ifs >> ignore >> ignore >> ignore >> ignore;

    std::vector<Vertex::PN> vertices(vertexCount);
    for (UINT i = 0; i < vertexCount; ++i)
    {
        ifs >> vertices[i].position.x >> vertices[i].position.y >> vertices[i].position.z;
        ifs >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;
    }

    ifs >> ignore >> ignore >> ignore;

    std::vector<UINT> indices(triangleCount * 3);
    for (UINT i = 0; i < triangleCount; ++i)
    {
        const UINT currentIndex = i * 3;
        ifs >> indices[currentIndex] >> indices[currentIndex + 1] >> indices[currentIndex + 2];
    }

    RenderAssetMap[ObjectType::Skull].submesh = Submesh(indices.size(), inoutIndices.size(), inoutVertices.size());
    inoutVertices.insert(inoutVertices.end(), vertices.begin(), vertices.end());
    inoutIndices.insert(inoutIndices.end(), indices.begin(), indices.end());
}

void LitSkullApp::InitShapeBuffer(std::vector<Vertex::PN>& inoutVertices, std::vector<UINT>& inoutIndices)
{
    auto addMeshData = [&](ObjectType objectType, const GeometryGenerator::MeshData& meshData)
    {
        std::vector<Vertex::PN> vertices(meshData.vertices.size());
        for (int i = 0; i < vertices.size(); ++i)
        {
            vertices[i].position = meshData.vertices[i].position;
            vertices[i].normal = meshData.vertices[i].normal;
        }

        std::vector<UINT> indices(meshData.indices.size());
        for (int i = 0; i < indices.size(); ++i)
        {
            indices[i] = meshData.indices[i];
        }

        RenderAssetMap[objectType].submesh = Submesh(indices.size(), inoutIndices.size(), inoutVertices.size());
        inoutVertices.insert(inoutVertices.end(), vertices.begin(), vertices.end());
        inoutIndices.insert(inoutIndices.end(), indices.begin(), indices.end());
    };

    addMeshData(ObjectType::Box, GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f));
    addMeshData(ObjectType::Grid, GeometryGenerator::CreateGrid(20.0f, 30.0f, 60, 40));
    addMeshData(ObjectType::Sphere, GeometryGenerator::CreateGeodesicSphere(0.5f, 3));
    addMeshData(ObjectType::Cylinder, GeometryGenerator::CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20));
}
