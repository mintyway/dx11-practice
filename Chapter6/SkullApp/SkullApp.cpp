#include "SkullApp.h"

#include "Data/Path.h"
#include "Rendering/VertexTypes.h"
#include "Utilities/Utility.h"

#include <fstream>
#include <vector>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    BaseEngine::Register<SkullApp>();

    if (!BaseEngine::Get()->Init(hInstance))
    {
        return 0;
    }

    BaseEngine::Get()->Run();

    return 0;
}

bool SkullApp::Init(HINSTANCE inInstanceHandle)
{
    if (!SphericalCamera::Init(inInstanceHandle))
    {
        return false;
    }

    LoadSkullMesh();
    InitShaderResource();

    return true;
}

void SkullApp::OnResize()
{
    SphericalCamera::OnResize();

    XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), GetAspectRatio(), 1.0f, 1000.0f));
}

SkullApp::SkullApp()
{
    SetCameraSphericalCoord(12.5f, -XM_PIDIV4, XM_PIDIV4);
}

void SkullApp::Update(float deltaSeconds)
{
    const XMVECTOR eyePosition = SphericalCoord::SphericalToCartesian(GetCameraSphericalCoord());
    const XMVECTOR focusPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(eyePosition, focusPosition, upDirection));
}

void SkullApp::Render()
{
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), Colors::White);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    D3D11_MAPPED_SUBRESOURCE mapped;
    immediateContext->Map(objectCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    XMStoreFloat4x4(static_cast<XMFLOAT4X4*>(mapped.pData), XMLoadFloat4x4(&skullWolrdMatrix) * XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix));
    immediateContext->Unmap(objectCB.Get(), 0);

    immediateContext->DrawIndexed(skullIndexCount, 0, 0);
    swapChain->Present(0, 0);
}

void SkullApp::LoadSkullMesh()
{
    std::ifstream ifs(Path::GetModelPath(L"skull.txt").c_str());

    UINT vertexCount = 0;
    UINT triangleCount = 0;
    std::string ignore;

    ifs >> ignore >> vertexCount;
    ifs >> ignore >> triangleCount;
    ifs >> ignore >> ignore >> ignore >> ignore;

    float normalX, normalY, normalZ;
    const Color black = Color::LinearColorToColor(Colors::Black);

    std::vector<Vertex> skullVertices(vertexCount);
    for (UINT i = 0; i < vertexCount; ++i)
    {
        ifs >> skullVertices[i].position.x >> skullVertices[i].position.y >> skullVertices[i].position.z;
        skullVertices[i].color = black;

        ifs >> normalX >> normalY >> normalZ; // 노말은 사용 안함.
    }

    ifs >> ignore >> ignore >> ignore;

    skullIndexCount = triangleCount * 3;
    std::vector<UINT> skullIndices(skullIndexCount);
    for (UINT i = 0; i < triangleCount; ++i)
    {
        const UINT currentIndex = i * 3;
        ifs >> skullIndices[currentIndex] >> skullIndices[currentIndex + 1] >> skullIndices[currentIndex + 2];
    }

    const CD3D11_BUFFER_DESC skullVertexBufferDesc(static_cast<UINT>(sizeof(Vertex) * skullVertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA skullVertexBufferInitData{skullVertices.data()};
    device->CreateBuffer(&skullVertexBufferDesc, &skullVertexBufferInitData, &skullVertexBuffer);

    constexpr UINT stride = static_cast<UINT>(sizeof(Vertex));
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, skullVertexBuffer.GetAddressOf(), &stride, &offset);

    const CD3D11_BUFFER_DESC skullIndexBufferDesc(static_cast<UINT>(sizeof(UINT) * skullIndices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA skullIndexBufferInitData{skullIndices.data()};
    device->CreateBuffer(&skullIndexBufferDesc, &skullIndexBufferInitData, &skullIndexBuffer);
    immediateContext->IASetIndexBuffer(skullIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    XMStoreFloat4x4(&skullWolrdMatrix, XMMatrixTranslation(0.0f, -2.5f, 0.0f));
}

void SkullApp::InitShaderResource()
{
    ComPtr<ID3DBlob> vertexShaderBlob;
    D3DReadFileToBlob(Path::GetShaderPath(L"Box_vs.cso").c_str(), &vertexShaderBlob);
    device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader);
    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);

    ComPtr<ID3DBlob> pixelShaderBlob;
    D3DReadFileToBlob(Path::GetShaderPath(L"Box_ps.cso").c_str(), &pixelShaderBlob);
    device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    device->CreateInputLayout(Vertex::Desc.data(), static_cast<UINT>(Vertex::Desc.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout);
    immediateContext->IASetInputLayout(inputLayout.Get());
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    const CD3D11_BUFFER_DESC objectCBDesc(sizeof(XMFLOAT4X4), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&objectCBDesc, nullptr, &objectCB);
    immediateContext->VSSetConstantBuffers(0, 1, objectCB.GetAddressOf());
}
