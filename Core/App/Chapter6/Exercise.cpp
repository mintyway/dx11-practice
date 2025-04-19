#include "Exercise.h"

#include <d3dcompiler.h>

#include "Core/Common/GeometryGenerator.h"
#include "Exercise/Chapter6.hpp"
#include "Rendering/VertexTypes.h"
#include "Utilities/Utility.h"

Exercise::Exercise()
{
    SetCameraSphericalCoord(7.5f, -XM_PIDIV2, XM_PIDIV4);
    SetZoomSpeed(0.01f);

    const XMMATRIX identityMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&pyramidWorldMatrix, identityMatrix);
    XMStoreFloat4x4(&viewMatrix, identityMatrix);
    XMStoreFloat4x4(&projectionMatrix, identityMatrix);
}


bool Exercise::Init(HINSTANCE inInstanceHandle)
{
    if (!EngineBase::Init(inInstanceHandle))
    {
        return false;
    }

    InitShader();
    CreateGeometry();

    return true;
}

LRESULT Exercise::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return EngineBase::HandleMessage(hWnd, message, wParam, lParam);
}

void Exercise::OnResize()
{
    EngineBase::OnResize();
    XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(XM_PIDIV4, GetAspectRatio(), 1.0f, 1000.0f));
}

void Exercise::Update(float deltaSeconds)
{
    const XMVECTOR eyePosition = SphericalCoord::SphericalToCartesian(GetCameraSphericalCoord());
    const XMVECTOR focusPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(eyePosition, focusPosition, upDirection));
}

void Exercise::Render()
{
    EngineBase::Render();

    immediateContext->ClearRenderTargetView(renderTargetView.Get(), Colors::White);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    constexpr UINT strid = sizeof(VertexWithColor);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &strid, &offset);
    immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    auto drawObject = [&](const XMFLOAT4X4& inWorldMatrix, const Submesh& inSubmesh)
    {
        D3D11_MAPPED_SUBRESOURCE mapped;
        CHECK_HR(immediateContext->Map(wvpMatrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped), L"");
        XMStoreFloat4x4(static_cast<XMFLOAT4X4*>(mapped.pData), XMLoadFloat4x4(&inWorldMatrix) * XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix));
        immediateContext->Unmap(wvpMatrixBuffer.Get(), 0);

        immediateContext->DrawIndexed(inSubmesh.indexCount, inSubmesh.startIndexLocation, inSubmesh.baseVertexLocation);
    };

    drawObject(pyramidWorldMatrix, pyramidSubmesh);
    drawObject(boxWorldMatrix, boxSubmesh);

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

    CHECK_HR(device->CreateInputLayout(VertexWithColor::Desc.data(), static_cast<UINT>(VertexWithColor::Desc.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout), L"");

    const CD3D11_BUFFER_DESC wvpMatrixBufferDesc(sizeof(XMFLOAT4X4), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&wvpMatrixBufferDesc, nullptr, &wvpMatrixBuffer);

    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    immediateContext->IASetInputLayout(inputLayout.Get());
    immediateContext->VSSetConstantBuffers(0, 1, wvpMatrixBuffer.GetAddressOf());
}

void Exercise::CreateGeometry()
{
    const std::vector<VertexWithColor> pyramidVertices = {
        VertexWithColor{XMFLOAT3(-1.0f, -1.0f, -1.0f), Color(0, 255, 0)},
        VertexWithColor{XMFLOAT3(-1.0f, -1.0f, 1.0f), Color(0, 255, 0)},
        VertexWithColor{XMFLOAT3(1.0f, -1.0f, 1.0f), Color(0, 255, 0)},
        VertexWithColor{XMFLOAT3(1.0f, -1.0f, -1.0f), Color(0, 255, 0)},
        VertexWithColor{XMFLOAT3(0.0f, 1.0f, 0.0f), Color(255, 0, 0)},
    };

    const std::vector<UINT> pyramidIndices = {
        0, 3, 1,
        3, 2, 1,
        0, 1, 4,
        3, 4, 2,
        1, 2, 4,
        0, 4, 3
    };

    const GeometryGenerator::MeshData boxMesh = GeometryGenerator::CreateBox(2.0f, 2.0f, 2.0f);
    std::vector<VertexWithColor> boxVertices(boxMesh.vertices.size());
    for (size_t i = 0; i < boxVertices.size(); ++i)
    {
        boxVertices[i].position = boxMesh.vertices[i].position;
        boxVertices[i].color = Color(255, 0, 255);
    }

    std::vector<VertexWithColor> vertices;
    vertices.insert(vertices.end(), pyramidVertices.begin(), pyramidVertices.end());
    vertices.insert(vertices.end(), boxVertices.begin(), boxVertices.end());

    std::vector<UINT> indices;
    indices.insert(indices.end(), pyramidIndices.begin(), pyramidIndices.end());
    indices.insert(indices.end(), boxMesh.indices.begin(), boxMesh.indices.end());

    pyramidSubmesh = Submesh{pyramidIndices.size(), 0, 0};
    boxSubmesh = Submesh{boxMesh.indices.size(), pyramidSubmesh.startIndexLocation + pyramidIndices.size(), pyramidSubmesh.baseVertexLocation + pyramidVertices.size()};

    XMStoreFloat4x4(&pyramidWorldMatrix, XMMatrixTranslation(-1.5f, 0.0f, 0.0f));
    XMStoreFloat4x4(&boxWorldMatrix, XMMatrixTranslation(1.5f, 0.0f, 0.0f));

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(VertexWithColor) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexBufferInitData{vertices.data()};
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, &vertexBufferInitData, &vertexBuffer), L"");

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * indices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexBufferInitData{indices.data()};
    CHECK_HR(device->CreateBuffer(&indexBufferDesc, &indexBufferInitData, &indexBuffer), L"");
}
