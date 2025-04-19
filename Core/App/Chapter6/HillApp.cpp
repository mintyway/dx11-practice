#include "HillApp.h"

#include "Common/GeometryGenerator.h"
#include "Data/SphericalCoord.h"
#include "Utilities/Utility.h"

#include <algorithm>
#include <d3dcompiler.h>

using namespace DirectX;

HillApp::HillApp()
{
    const XMMATRIX identityMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&worldMatrix, identityMatrix);
    XMStoreFloat4x4(&viewMatrix, identityMatrix);
    XMStoreFloat4x4(&projectionMatrix, identityMatrix);
}

bool HillApp::Init(HINSTANCE inInstanceHandle)
{
    if (!Super::Init(inInstanceHandle))
    {
        return false;
    }

    CreateGeometryBuffers();
    CreateShaders();

    return true;
}

void HillApp::OnResize()
{
    Super::OnResize();

    // 창의 크기가 수정되었으므로 종횡비를 갱신
    const XMMATRIX newProjectionMatrix = XMMatrixPerspectiveFovLH(0.25f * XM_PI, GetAspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&projectionMatrix, newProjectionMatrix);
}

void HillApp::OnMouseDown(WPARAM buttonState, int x, int y)
{
    lastMousePosition.x = x;
    lastMousePosition.y = y;
    SetCapture(windowHandle);
}

void HillApp::OnMouseUp(WPARAM buttonState, int x, int y)
{
    ReleaseCapture();
}

void HillApp::OnMouseMove(WPARAM buttonState, int x, int y)
{
    const POINT currentMousePosition{x, y};

    if (buttonState & MK_LBUTTON)
    {
        constexpr float rotationSpeed = XMConvertToRadians(0.25f); // 한 픽셀당 1/4도 회전
        const float dx = rotationSpeed * static_cast<float>(currentMousePosition.x - lastMousePosition.x);
        const float dy = rotationSpeed * -static_cast<float>(currentMousePosition.y - lastMousePosition.y); // 스크린 좌표는 위로갈수록 수가 작이지므로 -로 반전

        theta -= dx;
        phi = std::clamp(phi + dy, 0.1f, XM_PI - 0.1f);
    }
    else if (buttonState & MK_RBUTTON)
    {
        constexpr float zoomSpeed = 0.2f; // 1픽셀당 0.2만큼 근접 or 멀어짐
        const float dx = zoomSpeed * static_cast<float>(currentMousePosition.x - lastMousePosition.x);

        radius = std::clamp(radius - (dx), 100.0f, 600.0f);
    }

    lastMousePosition = currentMousePosition;
}

void HillApp::Update(float deltaSeconds)
{
    // 뷰 행렬 구축
    const XMVECTOR cameraPosition = SphericalCoord::SphericalToCartesian(SphericalCoord{radius, theta, phi});
    const XMVECTOR targetPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    const XMMATRIX newViewMatrix = XMMatrixLookAtLH(cameraPosition, targetPosition, upDirection);
    XMStoreFloat4x4(&viewMatrix, newViewMatrix);
}

void HillApp::Render()
{
    Super::Render();

    if (!immediateContext || !renderTargetView || !depthStencilView || !inputLayout || !vertexBuffer || !indexBuffer || !constantBuffer || !vertexShader || !pixelShader)
    {
        return;
    }

    // 클리어
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), LinearColors::Blue);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // input assembler 단계
    immediateContext->IASetInputLayout(inputLayout.Get());
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    constexpr UINT stride = sizeof(VertexWithLinearColor);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // WVP 행렬
    const XMMATRIX wvpMatrix = XMLoadFloat4x4(&worldMatrix) * XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);

    // constant buffer 업데이트
    immediateContext->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &wvpMatrix, 0, 0);
    immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    // 셰이더 연결
    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    /*Draw와 DrawIndexed의 차이
     * Draw
     * 버텍스버퍼 = { v0, v1, v2, v0, v2, v3}
     *
     * DrawIndexed
     * 버텍스버퍼 = { v0, v1, v2, v3}
     * 인덱스버퍼 = { 0, 1, 2, 0, 2, 3}
     *
     * Draw는 인덱스 버퍼없이 버텍스 버퍼만 사용하는데 버텍스를 중복정의하면서 그려야한다. (버텍스는 데이터 크기가 거대해 중복정의해서는 안된다.)
     * DrawIndexed는 중복되지 않는 버텍스 버퍼와 단순 순서정보만 나열된 인덱스 버퍼를 통해서 그린다.
     *
     * 따라서 99%의 경우 DrawIndexed를 사용한다.
     */
    // 셰이더 호출
    immediateContext->DrawIndexed(indexCount, 0, 0);

    swapChain->Present(0, 0);
}

void HillApp::CreateGeometryBuffers()
{
    GeometryGenerator::MeshData grid = GeometryGenerator::CreateGrid(160.0f, 160.0f, 50, 50);

    std::vector<VertexWithLinearColor> vertices(grid.vertices.size());
    for (size_t i = 0; i < grid.vertices.size(); ++i)
    {
        XMFLOAT3 position = grid.vertices[i].position;
        position.y = GetHeight(position.x, position.z);
        vertices[i].position = position;

        if (position.y < -10.0f)
        {
            vertices[i].linearColor = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
        }
        else if (position.y < 5.0f)
        {
            vertices[i].linearColor = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
        }
        else if (position.y < 12.0f)
        {
            vertices[i].linearColor = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
        }
        else if (position.y < 20.0f)
        {
            vertices[i].linearColor = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
        }
        else
        {
            vertices[i].linearColor = XMFLOAT4(0.48f, 0.96f, 0.62f, 1.0f);
        }
    }

    // 버텍스 버퍼 생성
    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(VertexWithLinearColor) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{vertices.data()};
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &vertexBuffer), L"버텍스 버퍼 생성에 실패했습니다.");

    // 인덱스 버퍼 생성
    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * grid.indices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{grid.indices.data()};
    CHECK_HR(device->CreateBuffer(&indexBufferDesc, &indexInitData, &indexBuffer), L"인덱스 버퍼 생성에 실패했습니다.");

    // 인덱스 카운트 업데이트
    indexCount = static_cast<UINT>(grid.indices.size());
}

void HillApp::CreateShaders()
{
    UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ComPtr<ID3DBlob> vertexShaderBlob;
    ComPtr<ID3DBlob> pixelShaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    auto checkShaderCompile = [&]
    {
        if (errorBlob)
        {
            MessageBoxA(nullptr, static_cast<char*>(errorBlob->GetBufferPointer()), "Shader Compile Error", MB_OK | MB_ICONERROR);
        }
    };

    // 버텍스 셰이더 컴파일 및 생성
    HRESULT hr = D3DCompileFromFile(L"Shader/Vertex/Box_vs.hlsl", nullptr, nullptr, "VS_Main", "vs_5_0", shaderFlags, 0, &vertexShaderBlob, &errorBlob);
    checkShaderCompile();
    CHECK_HR(hr, L"버텍스 셰이더 컴파일 실패");
    CHECK_HR(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader), L"버텍스 셰이더 생성 실패");

    // 픽셀 셰이더 컴파일 및 생성
    hr = D3DCompileFromFile(L"Shader/Pixel/Box_ps.hlsl", nullptr, nullptr, "PS_Main", "ps_5_0", shaderFlags, 0, &pixelShaderBlob, &errorBlob);
    checkShaderCompile();
    CHECK_HR(hr, L"픽셀 셰이더 컴파일 실패");
    CHECK_HR(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader), L"픽셀 셰이더 생성 실패");

    // 인풋 레이아웃 생성
    CHECK_HR(device->CreateInputLayout(VertexWithLinearColor::Desc.data(), static_cast<UINT>(VertexWithLinearColor::Desc.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout), L"인풋 레이아웃 생성 실패");

    // 상수버퍼 생성
    const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(XMMATRIX), D3D11_BIND_CONSTANT_BUFFER);
    device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
}
