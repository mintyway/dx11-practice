#include "LightingApp.h"

#include "Core/Core/GeometryGenerator.h"
#include "Core/Data/Path.h"
#include "Core/Data/SphericalCoord.h"
#include "Core/Rendering/VertexTypes.h"
#include "Core/Utilities/Utility.h"

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
}

bool LightingApp::Init(HINSTANCE inInstanceHandle)
{
    if (!Super::Init(inInstanceHandle))
    {
        return false;
    }

    if (!InitShaderResource())
    {
        return false;
    }

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
    const XMVECTOR eyePosition = SphericalCoord::SphericalToCartesian(GetCameraSphericalCoord());
    const XMVECTOR focusPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(eyePosition, focusPosition, upDirection));

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
        // vertices[i].color = Color(0, 0, 0, 255);
    }

    immediateContext->Unmap(wavesVertexBuffer.Get(), 0);
}

void LightingApp::Render()
{
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), Colors::LightSteelBlue);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    const XMMATRIX viewProjectionMatrix = XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);
    RenderObject(landVertexBuffer.Get(), landIndexBuffer.Get(), landWorldMatrix, viewProjectionMatrix, gridSubmesh.indexCount);
    RenderObject(wavesVertexBuffer.Get(), wavesIndexBuffer.Get(), wavesWorldMatrix, viewProjectionMatrix, wavesSubmesh.indexCount, true);

    swapChain->Present(0, 0);
}

void LightingApp::RenderObject(ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr, const XMFLOAT4X4& worldMatrix, FXMMATRIX viewProjectionMatrix, UINT indexCount, bool bUseWireframe)
{
    immediateContext->RSSetState(bUseWireframe ? wireframeRasterizerState.Get() : nullptr);

    XMFLOAT4X4 wvpMatrix;
    XMStoreFloat4x4(&wvpMatrix, XMLoadFloat4x4(&worldMatrix) * viewProjectionMatrix);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    CHECK_HR(immediateContext->Map(objectConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource), L"Failed to get mappedResource");
    *static_cast<XMFLOAT4X4*>(mappedResource.pData) = wvpMatrix;
    immediateContext->Unmap(objectConstantBuffer.Get(), 0);

    constexpr UINT stride = sizeof(Vertex);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);
    immediateContext->IASetIndexBuffer(indexBufferPtr, DXGI_FORMAT_R32_UINT, 0);

    immediateContext->DrawIndexed(indexCount, 0, 0);
}

bool LightingApp::InitShaderResource()
{
    ComPtr<ID3DBlob> vertexShaderBlob;
    D3DReadFileToBlob(Path::GetShaderPath(L"Light_vs.cso").c_str(), &vertexShaderBlob);
    device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader);
    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);

    ComPtr<ID3DBlob> pixelShaderBlob;
    D3DReadFileToBlob(Path::GetShaderPath(L"Light_ps.cso").c_str(), &pixelShaderBlob);
    device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    device->CreateInputLayout(Vertex::Desc.data(), static_cast<UINT>(Vertex::Desc.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout);
    immediateContext->IASetInputLayout(inputLayout.Get());
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    const CD3D11_BUFFER_DESC objectCBDesc(sizeof(XMFLOAT4X4), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&objectCBDesc, nullptr, &objectConstantBuffer);
    immediateContext->VSSetConstantBuffers(0, 1, objectConstantBuffer.GetAddressOf());

    return true;
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

        // if (position.y < -10.0f) // 모래 사장(밝은 노란색)
        // {
        //     vertices[i].color = Color::LinearColorToColor(XMVECTORF32{1.0f, 0.96f, 0.62f, 1.0f});
        // }
        // else if (position.y < 5.0f) // 저지대(밝은 녹색)
        // {
        //     vertices[i].color = Color::LinearColorToColor(XMVECTORF32{0.48f, 0.77f, 0.46f, 1.0f});
        // }
        // else if (position.y < 12.0f) // 중지대(어두운 녹색)
        // {
        //     vertices[i].color = Color::LinearColorToColor(XMVECTORF32{0.1f, 0.48f, 0.19f, 1.0f});
        // }
        // else if (position.y < 20.0f) // 고지대(갈색)
        // {
        //     vertices[i].color = Color::LinearColorToColor(XMVECTORF32{0.45f, 0.39f, 0.34f, 1.0f});
        // }
        // else // 눈(흰색)
        // {
        //     vertices[i].color = Color::LinearColorToColor(XMVECTORF32{1.0f, 1.0f, 1.0f, 1.0f});
        // }
    }

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{vertices.data()};
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &landVertexBuffer), L"Failed to create land vertext buffer", false);

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
    CHECK_HR(device->CreateBuffer(&indexBufferDesc, &indexInitData, &wavesIndexBuffer), L"Falied to create wave index buffer", false);

    return true;
}
