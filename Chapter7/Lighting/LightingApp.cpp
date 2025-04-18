#include "LightingApp.h"

#include "Core/Core/GeometryGenerator.h"
#include "Core/Data/Path.h"
#include "Core/Data/SphericalCoord.h"
#include "Core/Rendering/VertexTypes.h"
#include "Core/Utilities/Utility.h"
#include "Shaders/BasicShader.h"
#include "Type/ConstantBufferData.h"

#include <numbers>

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

LightingApp::LightingApp()
{
    SetCameraSphericalCoord(200.f, -XM_PIDIV4, XM_PIDIV4);
    SetZoomSpeed(0.25f);

    const XMMATRIX identityMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&landWorldMatrix, identityMatrix);
    XMStoreFloat4x4(&wavesWorldMatrix, identityMatrix);

    directionalLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    directionalLight.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    directionalLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    directionalLight.direction = XMFLOAT3(std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>);

    pointLight.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    pointLight.diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    pointLight.specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    pointLight.attenuation = XMFLOAT3(0.0f, 0.1f, 0.0f);
    pointLight.range = 25.0f;

    spotLight.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    spotLight.diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
    spotLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    spotLight.attenuation = XMFLOAT3(1.0f, 0.0f, 0.0f);
    spotLight.spot = 96.0f;
    spotLight.range = 10000.0f;

    landMaterial.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    landMaterial.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    landMaterial.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

    wavesMaterial.ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    wavesMaterial.diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    wavesMaterial.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);
}

bool LightingApp::Init(HINSTANCE inInstanceHandle)
{
    if (!Super::Init(inInstanceHandle))
    {
        return false;
    }

    basicShader = std::make_unique<BasicShader>(device.Get());
    basicShader->Bind(immediateContext.Get());

    if (!CreateGeometry())
    {
        return false;
    }

    return true;
}

void LightingApp::OnResize()
{
    Super::OnResize();

    XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(0.25f * XM_PI, GetAspectRatio(), 1.0f, 1000.0f));
}

void LightingApp::Update(float deltaSeconds)
{
    const XMVECTOR cameraPosition = SphericalCoord::SphericalToCartesian(GetCameraSphericalCoord());
    const XMVECTOR focusPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(cameraPosition, focusPosition, upDirection));
    XMStoreFloat3(&eyePosition, cameraPosition);

    static float elapsedTime = 0.0f;
    if (timer.GetTotalSeconds() - elapsedTime >= 0.1f)
    {
        elapsedTime += 0.1f;

        const UINT i = Math::GetRandomInt(5, 194);
        const UINT j = Math::GetRandomInt(5, 194);
        const float magnitude = Math::GetRandomFloat(0.5f, 1.0f);

        waves.Disturb(i, j, magnitude);
    }

    waves.Update(deltaSeconds);

    D3D11_MAPPED_SUBRESOURCE mapped;
    immediateContext->Map(wavesVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    Vertex* vertices = static_cast<Vertex*>(mapped.pData);
    for (UINT i = 0; i < waves.VertexCount(); ++i)
    {
        vertices[i].position = waves[static_cast<int>(i)];
        vertices[i].normal = waves.Normal(static_cast<int>(i));
    }

    immediateContext->Unmap(wavesVertexBuffer.Get(), 0);

    pointLight.position.x = 70.0f * std::cos(0.2f * static_cast<float>(timer.GetTotalSeconds()));
    pointLight.position.z = 70.0f * std::sin(0.2f * static_cast<float>(timer.GetTotalSeconds()));
    pointLight.position.y = std::max(GetHeight(pointLight.position.x, pointLight.position.z), -3.0f) + 10.0f;

    spotLight.position = eyePosition;
    XMStoreFloat3(&spotLight.direction, XMVector3Normalize(focusPosition - cameraPosition));
}

void LightingApp::Render()
{
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), Colors::LightSteelBlue);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    const XMMATRIX viewProjectionMatrix = XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);
    RenderObject(landVertexBuffer.Get(), landIndexBuffer.Get(), XMLoadFloat4x4(&landWorldMatrix), viewProjectionMatrix, landMaterial, gridSubmesh.indexCount);
    RenderObject(wavesVertexBuffer.Get(), wavesIndexBuffer.Get(), XMLoadFloat4x4(&wavesWorldMatrix), viewProjectionMatrix, wavesMaterial, wavesSubmesh.indexCount);

    swapChain->Present(0, 0);
}

void LightingApp::RenderObject(ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr, FXMMATRIX worldMatrix, CXMMATRIX viewProjectionMatrix, const Material& material, UINT indexCount)
{
    basicShader->SetMatrix(worldMatrix, viewProjectionMatrix);
    basicShader->SetMaterial(material);
    basicShader->SetLights(directionalLight, pointLight, spotLight);
    basicShader->SetEyePosition(eyePosition);
    basicShader->UpdateCBuffer(immediateContext.Get());

    constexpr UINT stride = sizeof(Vertex);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);
    immediateContext->IASetIndexBuffer(indexBufferPtr, DXGI_FORMAT_R32_UINT, 0);

    immediateContext->DrawIndexed(indexCount, 0, 0);
}

bool LightingApp::CreateGeometry()
{
    if (!CreateLandGeometry())
    {
        return false;
    }

    if (!CreateWaveGeometry())
    {
        return false;
    }

    return true;
}

bool LightingApp::CreateLandGeometry()
{
    const GeometryGenerator::MeshData grid = GeometryGenerator::CreateGrid(160.0f, 160.0f, 50, 50);
    gridSubmesh = Submesh(grid.indices.size(), 0, 0);

    std::vector<Vertex> vertices(grid.vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        XMFLOAT3 position = grid.vertices[i].position;
        position.y = GetHeight(position.x, position.z);
        vertices[i].position = position;
        vertices[i].normal = GetHillNormal(position.x, position.z);
    }

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{vertices.data()};
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &landVertexBuffer), L"Failed to create land vertex buffer", false);

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * gridSubmesh.indexCount), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{grid.indices.data()};
    CHECK_HR(device->CreateBuffer(&indexBufferDesc, &indexInitData, &landIndexBuffer), L"Failed to create land index buffer", false);

    return true;
}

bool LightingApp::CreateWaveGeometry()
{
    waves.Init(200, 200, 0.8f, 0.03f, 3.25f, 0.4f);
    const UINT wavesRowCount = waves.RowCount();
    const UINT wavesColumnCount = waves.ColumnCount();

    std::vector<UINT> indices(waves.TriangleCount() * 3);
    wavesSubmesh.indexCount = static_cast<UINT>(indices.size());

    int k = 0;
    for (UINT i = 0; i < wavesRowCount - 1; ++i)
    {
        for (UINT j = 0; j < wavesColumnCount - 1; ++j)
        {
            const UINT current = i * wavesColumnCount + j;
            const UINT next = current + 1;
            const UINT bottom = current + wavesColumnCount;
            const UINT bottomNext = bottom + 1;

            indices[k++] = current;
            indices[k++] = next;
            indices[k++] = bottom;

            indices[k++] = bottom;
            indices[k++] = next;
            indices[k++] = bottomNext;
        }
    }

    const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(Vertex) * waves.VertexCount(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, nullptr, &wavesVertexBuffer), L"Failed to create wave vertex buffer", false);

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * indices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{indices.data()};
    CHECK_HR(device->CreateBuffer(&indexBufferDesc, &indexInitData, &wavesIndexBuffer), L"Failed to create wave index buffer", false);

    return true;
}
