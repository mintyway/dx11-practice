#include "Exercise.h"

#include "Core/GeometryGenerator.h"
#include "Rendering/VertexTypes.h"
#include "Utilities/Utility.h"

Exercise::Exercise()
{
    SetCameraSphericalCoord(10.0f, -XM_PIDIV2, XM_PIDIV2);

    const XMMATRIX identityMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&worldMatrix, identityMatrix);
    XMStoreFloat4x4(&viewMatrix, identityMatrix);
    XMStoreFloat4x4(&projectionMatrix, identityMatrix);
    XMStoreFloat4x4(&boxWolrdMatrix, identityMatrix);
}


bool Exercise::Init(HINSTANCE inInstanceHandle)
{
    if (!BaseEngine::Init(inInstanceHandle))
    {
        return false;
    }

    InitShader();
    CreateGeometry();

    return true;
}

LRESULT Exercise::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return BaseEngine::HandleMessage(hWnd, message, wParam, lParam);
}

void Exercise::OnResize()
{
    BaseEngine::OnResize();
    XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(XM_PIDIV4, GetAspectRatio(), 1.0f, 1000.0f));
}

void Exercise::Update(float deltaSeconds)
{
    const XMVECTOR eyePosition = Math::SphericalToCartesian(GetCameraRadius(), GetCameraTheta(), GetCameraPhi());
    const XMVECTOR focusPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(eyePosition, focusPosition, upDirection));
}

void Exercise::Render()
{
    BaseEngine::Render();

    immediateContext->ClearRenderTargetView(renderTargetView.Get(), Colors::White);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    D3D11_MAPPED_SUBRESOURCE mapped;
    CHECK_HR(immediateContext->Map(wvpMatrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped), L"");
    XMStoreFloat4x4(static_cast<XMFLOAT4X4*>(mapped.pData), XMLoadFloat4x4(&worldMatrix) * XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix));
    immediateContext->Unmap(wvpMatrixBuffer.Get(), 0);

    constexpr UINT strid = sizeof(Vertex);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &strid, &offset);

    immediateContext->RSSetState(wireframeRasterizerState.Get());

    immediateContext->Draw(vertexCount, 0);

    // for (const XMFLOAT3& pointPosition : pointPositions)
    // {
    //     XMFLOAT4X4 pointWorldMatrix;
    //     XMStoreFloat4x4(&pointWorldMatrix, XMMatrixTranslationFromVector(XMLoadFloat3(&pointPosition)));
    //     DrawObject(geodesicSphereVertexBuffer.Get(), geodesicSphereIndexBuffer.Get(), pointWorldMatrix, geodesicSphereIndexCount);
    // }

    swapChain->Present(0, 0);
}

void Exercise::InitShader()
{
    UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ComPtr<ID3DBlob> vertexShaderBlob;
    ComPtr<ID3DBlob> vertexShaderErrorBlob;
    CHECK_HR(D3DCompileFromFile(L"Shader/Vertex/Box_vs.hlsl", nullptr, nullptr, "VS_Main", "vs_5_0", shaderFlags, 0, &vertexShaderBlob, &vertexShaderErrorBlob), L"");
    CHECK_HR(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader), L"");

    ComPtr<ID3DBlob> pixelShaderBlob;
    ComPtr<ID3DBlob> pixelShaderErrorBlob;
    CHECK_HR(D3DCompileFromFile(L"Shader/Pixel/Box_ps.hlsl", nullptr, nullptr, "PS_Main", "ps_5_0", shaderFlags, 0, &pixelShaderBlob, &pixelShaderErrorBlob), L"");
    CHECK_HR(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader), L"");

    CHECK_HR(device->CreateInputLayout(VertexDesc.data(), static_cast<UINT>(VertexDesc.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout), L"");

    const CD3D11_BUFFER_DESC wvpMatrixBufferDesc(sizeof(XMFLOAT4X4), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&wvpMatrixBufferDesc, nullptr, &wvpMatrixBuffer);

    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    immediateContext->IASetInputLayout(inputLayout.Get());

    immediateContext->VSSetConstantBuffers(0, 1, wvpMatrixBuffer.GetAddressOf());
}

void Exercise::CreateGeometry()
{
    const std::array<Vertex, 8> vertices = {
        Vertex{XMFLOAT3(-3.0f, -2.0f, 0.0f), XMFLOAT4(Colors::Black)},
        Vertex{XMFLOAT3(-2.0f, 1.0f, 0.0f), XMFLOAT4(Colors::Black)},
        Vertex{XMFLOAT3(-1.0f, -1.5f, 0.0f), XMFLOAT4(Colors::Black)},
        Vertex{XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(Colors::Black)},
        Vertex{XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT4(Colors::Black)},
        Vertex{XMFLOAT3(2.0f, 0.5f, 0.0f), XMFLOAT4(Colors::Black)},
        Vertex{XMFLOAT3(3.0f, -0.5f, 0.0f), XMFLOAT4(Colors::Black)},
        Vertex{XMFLOAT3(4.0f, 1.0f, 0.0f), XMFLOAT4(Colors::Black)},
    };
    vertexCount = static_cast<UINT>(vertices.size());

    pointPositions = {
        XMFLOAT3(-3.0f, -2.0f, 0.0f),
        XMFLOAT3(-2.0f, 1.0f, 0.0f),
        XMFLOAT3(-1.0f, -1.5f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.0f, -1.0f, 0.0f),
        XMFLOAT3(2.0f, 0.5f, 0.0f),
        XMFLOAT3(3.0f, -0.5f, 0.0f),
        XMFLOAT3(4.0f, 1.0f, 0.0f)
    };

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexBufferInitData{vertices.data()};
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, &vertexBufferInitData, &vertexBuffer), L"");

    CreateGeodesicSphere();
}

void Exercise::CreateGeodesicSphere()
{
    const GeometryGenerator::MeshData geodesicSphereMesh = GeometryGenerator::CreateGeodesicSphere(0.1f, 2);
    geodesicSphereIndexCount = static_cast<UINT>(geodesicSphereMesh.indices.size());

    std::vector<Vertex> vertices(geodesicSphereMesh.vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        vertices[i].pos = geodesicSphereMesh.vertices[i].position;
        vertices[i].color = XMFLOAT4(Colors::Black);
    }

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex) * vertices.size()), D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{vertices.data()};
    device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &geodesicSphereVertexBuffer);

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * geodesicSphereMesh.indices.size()), D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{geodesicSphereMesh.indices.data()};
    device->CreateBuffer(&indexBufferDesc, &indexInitData, &geodesicSphereIndexBuffer);
}

void Exercise::DrawObject(ID3D11Buffer* inVertexBuffer, ID3D11Buffer* inIndexBuffer, const XMFLOAT4X4& inWorldMatrix, UINT inIndexCount)
{
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_MAPPED_SUBRESOURCE mapped;
    CHECK_HR(immediateContext->Map(wvpMatrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped), L"");
    XMStoreFloat4x4(static_cast<XMFLOAT4X4*>(mapped.pData), XMLoadFloat4x4(&inWorldMatrix) * XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix));
    immediateContext->Unmap(wvpMatrixBuffer.Get(), 0);

    constexpr UINT strid = sizeof(Vertex);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, &inVertexBuffer, &strid, &offset);
    immediateContext->IASetIndexBuffer(inIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    immediateContext->DrawIndexed(inIndexCount, 0, 0);
}
