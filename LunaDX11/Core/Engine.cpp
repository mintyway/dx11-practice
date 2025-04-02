#include "Engine.h"

#include "Utility.h"

#include <algorithm>

Engine::Engine()
{
    const XMMATRIX identityMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&worldMatrix, identityMatrix);
    XMStoreFloat4x4(&viewMatrix, identityMatrix);
    XMStoreFloat4x4(&projectionMatrix, identityMatrix);

    boxVertices =
    {
        Vertex{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(Colors::White)},
        Vertex{XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT3(Colors::Black)},
        Vertex{XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT3(Colors::Red)},
        Vertex{XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT3(Colors::Green)},
        Vertex{XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT3(Colors::Blue)},
        Vertex{XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT3(Colors::Yellow)},
        Vertex{XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT3(Colors::Cyan)},
        Vertex{XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT3(Colors::Magenta)}
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
}

bool Engine::Init(HINSTANCE inInstanceHandle)
{
    if (!Super::Init(inInstanceHandle))
    {
        return false;
    }

    CreateGeometryBuffers();
    CreateShaders();

    return true;
}

void Engine::OnResize()
{
    Super::OnResize();

    // 창의 크기가 수정되었으므로 종횡비를 갱신
    const XMMATRIX newProjectionMatrix = XMMatrixPerspectiveFovLH(0.25f * XM_PI, GetAspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&projectionMatrix, newProjectionMatrix);
}

void Engine::OnMouseDown(WPARAM buttonState, int x, int y)
{
    lastMousePosition.x = x;
    lastMousePosition.y = y;
    SetCapture(windowHandle);
}

void Engine::OnMouseUp(WPARAM buttonState, int x, int y)
{
    ReleaseCapture();
}

void Engine::OnMouseMove(WPARAM buttonState, int x, int y)
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

void Engine::Update(float deltaSeconds)
{
    // 뷰 행렬 구축
    const XMVECTOR cameraPosition = Math::SphericalToCartesian(radius, theta, phi);
    const XMVECTOR targetPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    const XMMATRIX newViewMatrix = XMMatrixLookAtLH(cameraPosition, targetPosition, upDirection);
    XMStoreFloat4x4(&viewMatrix, newViewMatrix);
}

void Engine::Render()
{
    Super::Render();

    if (!immediateContext || !renderTargetView || !depthStencilView || !inputLayout || !boxVertexBuffer || !boxIndexBuffer || !constantBuffer || !vertexShader || !pixelShader)
    {
        return;
    }

    // 클리어
    immediateContext->ClearRenderTargetView(renderTargetView, Colors::Blue);
    immediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // input assembler 단계
    immediateContext->IASetInputLayout(inputLayout.Get());
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    constexpr UINT stride = sizeof(Vertex);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, boxVertexBuffer.GetAddressOf(), &stride, &offset);
    immediateContext->IASetIndexBuffer(boxIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // WVP 행렬
    const XMMATRIX wvpMatrix = XMLoadFloat4x4(&worldMatrix) * XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);

    // constant buffer 업데이트
    immediateContext->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &wvpMatrix, 0, 0);
    immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    // 셰이더 연결
    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    immediateContext->DrawIndexed(36, 0, 0);

    swapChain->Present(0, 0);
}

void Engine::CreateGeometryBuffers()
{
    // 버텍스 버퍼 생성
    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex) * boxVertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{boxVertices.data()};
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &boxVertexBuffer), L"버텍스 버퍼 생성에 실패했습니다.");

    // 인덱스 버퍼 생성
    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * boxIndices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{boxIndices.data()};
    CHECK_HR(device->CreateBuffer(&indexBufferDesc, &indexInitData, &boxIndexBuffer), L"인덱스 버퍼 생성에 실패했습니다.");
}

void Engine::CreateShaders()
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
    CHECK_HR(device->CreateInputLayout(VertexDesc.data(), static_cast<UINT>(VertexDesc.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout), L"인풋 레이아웃 생성 실패");

    // 상수버퍼 생성
    const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(XMMATRIX), D3D11_BIND_CONSTANT_BUFFER);
    device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
}
