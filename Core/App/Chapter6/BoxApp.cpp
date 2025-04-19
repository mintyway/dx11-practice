#include "BoxApp.h"

#include <algorithm>
#include <d3dcompiler.h>

#include "Core/Common/Timer.h"
#include "Data/SphericalCoord.h"
#include "Exercise/Chapter6.hpp"
#include "Utilities/Utility.h"

BoxApp::BoxApp()
{
    const XMMATRIX identityMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&worldMatrix, identityMatrix);
    XMStoreFloat4x4(&viewMatrix, identityMatrix);
    XMStoreFloat4x4(&projectionMatrix, identityMatrix);
}

bool BoxApp::Init(HINSTANCE inInstanceHandle)
{
    if (!Super::Init(inInstanceHandle))
    {
        return false;
    }

    CreateGeometryBuffers();
    CreateShaders();

    return true;
}

void BoxApp::OnResize()
{
    Super::OnResize();

    // 창의 크기가 수정되었으므로 종횡비를 갱신
    const XMMATRIX newProjectionMatrix = XMMatrixPerspectiveFovLH(0.25f * XM_PI, GetAspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&projectionMatrix, newProjectionMatrix);
}

void BoxApp::OnMouseDown(WPARAM buttonState, int x, int y)
{
    lastMousePosition.x = x;
    lastMousePosition.y = y;
    SetCapture(windowHandle);
}

void BoxApp::OnMouseUp(WPARAM buttonState, int x, int y)
{
    ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM buttonState, int x, int y)
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
        constexpr float zoomSpeed = 0.01f; // 1픽셀당 0.01만큼 근접 or 멀어짐
        const float dx = zoomSpeed * static_cast<float>(currentMousePosition.x - lastMousePosition.x);

        radius = std::clamp(radius - (dx), 3.0f, 15.0f);
    }

    lastMousePosition = currentMousePosition;
}

void BoxApp::Update(float deltaSeconds)
{
    // 뷰 행렬 구축
    const XMVECTOR cameraPosition = SphericalCoord::SphericalToCartesian(SphericalCoord{radius, theta, phi});
    const XMVECTOR targetPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    const XMMATRIX newViewMatrix = XMMatrixLookAtLH(cameraPosition, targetPosition, upDirection);
    XMStoreFloat4x4(&viewMatrix, newViewMatrix);
}

void BoxApp::Render()
{
    Super::Render();

    if (!immediateContext || !renderTargetView || !depthStencilView || !inputLayout || !boxIndexBuffer || !boxPositionVertexBuffer || !boxColorVertexBuffer || !wvpMatrixBuffer || !vertexShader || !pixelShader)
    {
        return;
    }

    // 클리어
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), Colors::Blue);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // input assembler 단계
    immediateContext->IASetInputLayout(inputLayout.Get());
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D11Buffer* const vertexBuffers[] = {boxPositionVertexBuffer.Get(), boxColorVertexBuffer.Get()};
    constexpr UINT strides[] = {sizeof(Chapter6::PositionVertex), sizeof(Chapter6::ColorVertex)};
    constexpr UINT offsets[] = {0, 0};
    immediateContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
    immediateContext->IASetIndexBuffer(boxIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // WVP 행렬
    // constant buffer 업데이트
    const XMMATRIX wvpMatrix = XMLoadFloat4x4(&worldMatrix) * XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);
    immediateContext->UpdateSubresource(wvpMatrixBuffer.Get(), 0, nullptr, &wvpMatrix, 0, 0);

    D3D11_MAPPED_SUBRESOURCE mapped;
    immediateContext->Map(timeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    *static_cast<float*>(mapped.pData) = static_cast<float>(timer->GetTotalSeconds());
    immediateContext->Unmap(timeBuffer.Get(), 0);

    ID3D11Buffer* constantBuffers[] = {wvpMatrixBuffer.Get(), timeBuffer.Get()};
    immediateContext->VSSetConstantBuffers(0, 2, constantBuffers);

    // 셰이더 연결
    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    immediateContext->DrawIndexed(static_cast<UINT>(boxIndices.size()), 0, 0);

    swapChain->Present(0, 0);
}

void BoxApp::CreateGeometryBuffers()
{
    std::array<Chapter6::PositionVertex, 8> positionVertex =
    {
        Chapter6::PositionVertex{XMFLOAT3(-1.0f, -1.0f, -1.0f)},
        Chapter6::PositionVertex{XMFLOAT3(-1.0f, +1.0f, -1.0f)},
        Chapter6::PositionVertex{XMFLOAT3(+1.0f, +1.0f, -1.0f)},
        Chapter6::PositionVertex{XMFLOAT3(+1.0f, -1.0f, -1.0f)},
        Chapter6::PositionVertex{XMFLOAT3(-1.0f, -1.0f, +1.0f)},
        Chapter6::PositionVertex{XMFLOAT3(-1.0f, +1.0f, +1.0f)},
        Chapter6::PositionVertex{XMFLOAT3(+1.0f, +1.0f, +1.0f)},
        Chapter6::PositionVertex{XMFLOAT3(+1.0f, -1.0f, +1.0f)}
    };

    std::array<Chapter6::ColorVertex, 8> colorVertex =
    {
        Chapter6::ColorVertex{XMFLOAT4(Colors::White)},
        Chapter6::ColorVertex{XMFLOAT4(Colors::Black)},
        Chapter6::ColorVertex{XMFLOAT4(Colors::Red)},
        Chapter6::ColorVertex{XMFLOAT4(Colors::Green)},
        Chapter6::ColorVertex{XMFLOAT4(Colors::Blue)},
        Chapter6::ColorVertex{XMFLOAT4(Colors::Yellow)},
        Chapter6::ColorVertex{XMFLOAT4(Colors::Cyan)},
        Chapter6::ColorVertex{XMFLOAT4(Colors::Magenta)}
    };

    boxIndices =
    {
        // 앞면
        0, 1, 2,
        0, 2, 3,

        // 뒷면
        7, 6, 5,
        7, 5, 4,

        // 좌측면
        4, 5, 1,
        4, 1, 0,

        // 우측면
        3, 2, 6,
        3, 6, 7,

        // 윗면
        1, 5, 6,
        1, 6, 2,

        // 밑면
        4, 0, 3,
        4, 3, 7
    };

    // 버텍스 버퍼 생성
    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Chapter6::PositionVertex) * positionVertex.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA positionVertexInitData{positionVertex.data()};
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, &positionVertexInitData, &boxPositionVertexBuffer), L"위치 버텍스 버퍼 생성에 실패했습니다.");

    const CD3D11_BUFFER_DESC colorBufferDesc(static_cast<UINT>(sizeof(Chapter6::ColorVertex) * colorVertex.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA colorVertexInitData{colorVertex.data()};
    CHECK_HR(device->CreateBuffer(&colorBufferDesc, &colorVertexInitData, &boxColorVertexBuffer), L"색상 버텍스 버퍼 생성에 실패했습니다.");

    // 인덱스 버퍼 생성
    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * boxIndices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{boxIndices.data()};
    CHECK_HR(device->CreateBuffer(&indexBufferDesc, &indexInitData, &boxIndexBuffer), L"인덱스 버퍼 생성에 실패했습니다.");
}

void BoxApp::CreateShaders()
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
    HRESULT hr = D3DCompileFromFile(L"Shader/Vertex/6-6_vs.hlsl", nullptr, nullptr, "VS_Main", "vs_5_0", shaderFlags, 0, &vertexShaderBlob, &errorBlob);
    checkShaderCompile();
    CHECK_HR(hr, L"버텍스 셰이더 컴파일 실패");
    CHECK_HR(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader), L"버텍스 셰이더 생성 실패");

    // 픽셀 셰이더 컴파일 및 생성
    hr = D3DCompileFromFile(L"Shader/Pixel/Box_ps.hlsl", nullptr, nullptr, "PS_Main", "ps_5_0", shaderFlags, 0, &pixelShaderBlob, &errorBlob);
    checkShaderCompile();
    CHECK_HR(hr, L"픽셀 셰이더 컴파일 실패");
    CHECK_HR(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader), L"픽셀 셰이더 생성 실패");

    // 인풋 레이아웃 생성
    CHECK_HR(device->CreateInputLayout(Chapter6::BoxVertexDesc.data(), static_cast<UINT>(Chapter6::BoxVertexDesc.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout), L"인풋 레이아웃 생성 실패");

    // 상수버퍼 생성
    const CD3D11_BUFFER_DESC wvpMatrixBufferDesc(sizeof(XMMATRIX), D3D11_BIND_CONSTANT_BUFFER);
    device->CreateBuffer(&wvpMatrixBufferDesc, nullptr, &wvpMatrixBuffer);

    // 시간 상수 버퍼 생성
    const CD3D11_BUFFER_DESC timeBufferDesc(16, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&timeBufferDesc, nullptr, &timeBuffer);
}
