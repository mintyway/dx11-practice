#include "WavesApp.h"

#include "Core/GeometryGenerator.h"
#include "Utilities/Utility.h"

#include <algorithm>

WavesApp::WavesApp()
{
    const XMMATRIX identityMatrix = XMMatrixIdentity();

    XMStoreFloat4x4(&viewMatrix, identityMatrix);
    XMStoreFloat4x4(&projectionMatrix, identityMatrix);

    XMStoreFloat4x4(&landWorldMatrix, identityMatrix);
    XMStoreFloat4x4(&wavesWorldMatrix, identityMatrix);
}

bool WavesApp::Init(HINSTANCE inInstanceHandle)
{
    if (!Super::Init(inInstanceHandle))
    {
        return false;
    }

    if (!CreateShaders())
    {
        return false;
    }

    if (!BindShader())
    {
        return false;
    }

    if (!CreateGeometryBuffers())
    {
        return false;
    }

    return true;
}

void WavesApp::OnResize()
{
    Super::OnResize();

    // 창의 크기가 수정되었으므로 투영 행렬의 종횡비를 갱신한다.
    XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(0.25f * XM_PI, GetAspectRatio(), 1.0f, 1000.0f));
}

void WavesApp::OnMouseDown(WPARAM buttonState, int x, int y)
{
    lastMousePosition.x = x;
    lastMousePosition.y = y;
    SetCapture(windowHandle);
}

void WavesApp::OnMouseUp(WPARAM buttonState, int x, int y)
{
    ReleaseCapture();
}

void WavesApp::OnMouseMove(WPARAM buttonState, int x, int y)
{
    const POINT currentMousePosition{x, y};

    if (buttonState & MK_LBUTTON)
    {
        constexpr float rotationSpeed = XMConvertToRadians(0.25f); // 한 픽셀당 1/4도 회전
        const float dx = rotationSpeed * static_cast<float>(currentMousePosition.x - lastMousePosition.x);
        const float dy = rotationSpeed * static_cast<float>(currentMousePosition.y - lastMousePosition.y);

        theta -= dx;
        phi = std::clamp(phi - dy, 0.1f, XM_PI - 0.1f);
    }
    else if (buttonState & MK_RBUTTON)
    {
        constexpr float zoomSpeed = 0.1f; // 1픽셀당 0.1만큼 근접 or 멀어짐
        const float dx = zoomSpeed * static_cast<float>(currentMousePosition.x - lastMousePosition.x);

        radius = std::clamp(radius - (dx), 50.0f, 300.0f);
    }

    lastMousePosition = currentMousePosition;
}

void WavesApp::Update(float deltaSeconds)
{
    // 뷰 행렬을 구축한다.
    const XMVECTOR eyePosition = Math::SphericalToCartesian(radius, theta, phi);
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

    D3D11_MAPPED_SUBRESOURCE mappedData;
    CHECK_HR(immediateContext->Map(wavesVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData), L"Failed to get wave vertex buffer map");

    VertexWithLinearColor* vertices = static_cast<VertexWithLinearColor*>(mappedData.pData);
    for (UINT i = 0; i < waves.VertexCount(); ++i)
    {
        vertices[i].position = waves[static_cast<int>(i)];
        vertices[i].linearColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    immediateContext->Unmap(wavesVertexBuffer.Get(), 0);
}

void WavesApp::Render()
{
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), Colors::LightSteelBlue);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    const XMMATRIX vpMatrix = XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);
    auto renderObject = [&](ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr, const XMFLOAT4X4& worldMatrix, UINT indexCount, bool bUseWireframe = false)
    {
        immediateContext->RSSetState(bUseWireframe ? wireframeRasterizerState.Get() : nullptr);

        XMFLOAT4X4 wvpMatrix;
        XMStoreFloat4x4(&wvpMatrix, XMLoadFloat4x4(&worldMatrix) * vpMatrix);

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        CHECK_HR(immediateContext->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource), L"Failed to get mappedResource");
        *static_cast<XMFLOAT4X4*>(mappedResource.pData) = wvpMatrix;
        immediateContext->Unmap(constantBuffer.Get(), 0);

        constexpr UINT stride = sizeof(VertexWithLinearColor);
        constexpr UINT offset = 0;
        immediateContext->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);
        immediateContext->IASetIndexBuffer(indexBufferPtr, DXGI_FORMAT_R32_UINT, 0);

        immediateContext->DrawIndexed(indexCount, 0, 0);
    };

    renderObject(landVertexBuffer.Get(), landIndexBuffer.Get(), landWorldMatrix, gridSubmesh.indexCount);
    renderObject(wavesVertexBuffer.Get(), wavesIndexBuffer.Get(), wavesWorldMatrix, wavesSubmesh.indexCount, true);

    swapChain->Present(0, 0);
}

bool WavesApp::CreateShaders()
{
    UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    auto checkShaderCompile = [&](HRESULT hr, ID3DBlob* errorBlob)
    {
        if (FAILED(hr) && errorBlob)
        {
            MessageBoxA(nullptr, static_cast<char*>(errorBlob->GetBufferPointer()), "Failed to shader compile", MB_OK | MB_ICONERROR);
        }
    };

    ComPtr<ID3DBlob> vertexShaderBlob;
    ComPtr<ID3DBlob> vertexShaderErrorBlob;
    checkShaderCompile(D3DCompileFromFile(L"Shader/Vertex/Box_vs.hlsl", nullptr, nullptr, "VS_Main", "vs_5_0", shaderFlags, 0, &vertexShaderBlob, &vertexShaderErrorBlob), vertexShaderErrorBlob.Get());
    CHECK_HR(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader), L"Failed to create vertex shader", false);

    ComPtr<ID3DBlob> pixelShaderBlob;
    ComPtr<ID3DBlob> pixelShaderErrorBlob;
    checkShaderCompile(D3DCompileFromFile(L"Shader/Pixel/Box_ps.hlsl", nullptr, nullptr, "PS_Main", "ps_5_0", shaderFlags, 0, &pixelShaderBlob, &pixelShaderErrorBlob), pixelShaderErrorBlob.Get());
    CHECK_HR(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader), L"Failed to create pixel shader", false);

    CHECK_HR(device->CreateInputLayout(VertexWithLinearColor::Desc.data(), static_cast<UINT>(VertexWithLinearColor::Desc.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout), L"Failed to create input layout", false);

    const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(XMFLOAT4X4), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    CHECK_HR(device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer), L"Failed to create constant buffer", false);

    return true;
}

bool WavesApp::BindShader()
{
    immediateContext->RSSetState(wireframeRasterizerState.Get());

    immediateContext->IASetInputLayout(inputLayout.Get());
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    return true;
}

bool WavesApp::CreateGeometryBuffers()
{
    if (!CreateLandGeometryBuffer())
    {
        return false;
    }

    if (!CreateWaveGeometryBuffer())
    {
        return false;
    }

    return true;
}

bool WavesApp::CreateLandGeometryBuffer()
{
    const GeometryGenerator::MeshData grid = GeometryGenerator::CreateGrid(160.0f, 160.0f, 50, 50);
    gridSubmesh = Submesh(grid.indices.size(), 0, 0);

    std::vector<VertexWithLinearColor> vertices(grid.vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        XMFLOAT3 position = grid.vertices[i].position;
        position.y = GetHeight(position.x, position.z);
        vertices[i].position = position;

        if (position.y < -10.0f) // 모래 사장(밝은 노란색)
        {
            vertices[i].linearColor = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
        }
        else if (position.y < 5.0f) // 저지대(밝은 녹색)
        {
            vertices[i].linearColor = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
        }
        else if (position.y < 12.0f) // 중지대(어두운 녹색)
        {
            vertices[i].linearColor = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
        }
        else if (position.y < 20.0f) // 고지대(갈색)
        {
            vertices[i].linearColor = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
        }
        else // 눈(흰색)
        {
            vertices[i].linearColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(VertexWithLinearColor) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{vertices.data()};
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &landVertexBuffer), L"Failed to create land vertext buffer", false);

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * gridSubmesh.indexCount), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{grid.indices.data()};
    CHECK_HR(device->CreateBuffer(&indexBufferDesc, &indexInitData, &landIndexBuffer), L"Failed to create land index buffer", false);

    return true;
}

bool WavesApp::CreateWaveGeometryBuffer()
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

    const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexWithLinearColor) * waves.VertexCount(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, nullptr, &wavesVertexBuffer), L"Failed to create wave vertex buffer", false);

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * indices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{indices.data()};
    CHECK_HR(device->CreateBuffer(&indexBufferDesc, &indexInitData, &wavesIndexBuffer), L"Falied to create wave index buffer", false);

    return true;
}
