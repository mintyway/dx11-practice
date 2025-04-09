#include "MultiDrawApp.h"

#include "Core/GeometryGenerator.h"
#include "Utilities/Utility.h"

#include <algorithm>

MultiDrawApp::MultiDrawApp()
{
    const XMMATRIX identityMatrix = XMMatrixIdentity();

    XMStoreFloat4x4(&viewMatrix, identityMatrix);
    XMStoreFloat4x4(&projectionMatrix, identityMatrix);

    XMStoreFloat4x4(&gridWorldMatrix, identityMatrix);

    const XMMATRIX boxScaleMatrix = XMMatrixScaling(2.0f, 1.0f, 2.0f);
    const XMMATRIX boxTranslationMatrix = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
    XMStoreFloat4x4(&boxWorldMatrix, boxScaleMatrix * boxTranslationMatrix);

    const XMMATRIX centerSphereScaleMatrix = XMMatrixScaling(2.0f, 2.0f, 2.0f);
    const XMMATRIX centerSphereTranslationMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
    XMStoreFloat4x4(&centerSphereWorldMatrix, centerSphereScaleMatrix * centerSphereTranslationMatrix);

    for (size_t i = 0; i < cylinderWorldMatrices.size() / 2; ++i)
    {
        XMStoreFloat4x4(&cylinderWorldMatrices[i * 2], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
        XMStoreFloat4x4(&cylinderWorldMatrices[i * 2 + 1], XMMatrixTranslation(5.0f, 1.5f, -10.f + i * 5.0f));
    }

    for (size_t i = 0; i < sphereWorldMatrices.size() / 2; ++i)
    {
        XMStoreFloat4x4(&sphereWorldMatrices[i * 2], XMMatrixTranslation(-5.0f, 3.5f, -10.f + i * 5.0f));
        XMStoreFloat4x4(&sphereWorldMatrices[i * 2 + 1], XMMatrixTranslation(5.0f, 3.5f, -10.f + i * 5.0f));
    }
}

bool MultiDrawApp::Init(HINSTANCE inInstanceHandle)
{
    if (!Super::Init(inInstanceHandle))
    {
        return false;
    }

    CreateGeometryBuffers();
    CreateShaders();
    BindShader();

    return true;
}

void MultiDrawApp::OnResize()
{
    Super::OnResize();

    // 창의 크기가 수정되었으므로 종횡비를 갱신
    const XMMATRIX newProjectionMatrix = XMMatrixPerspectiveFovLH(0.25f * XM_PI, GetAspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&projectionMatrix, newProjectionMatrix);
}

void MultiDrawApp::OnMouseDown(WPARAM buttonState, int x, int y)
{
    lastMousePosition.x = x;
    lastMousePosition.y = y;
    SetCapture(windowHandle);
}

void MultiDrawApp::OnMouseUp(WPARAM buttonState, int x, int y)
{
    ReleaseCapture();
}

void MultiDrawApp::OnMouseMove(WPARAM buttonState, int x, int y)
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
        constexpr float zoomSpeed = 0.1f; // 1픽셀당 0.1만큼 근접 or 멀어짐
        const float dx = zoomSpeed * static_cast<float>(currentMousePosition.x - lastMousePosition.x);

        radius = std::clamp(radius - (dx), 15.0f, 45.0f);
    }

    lastMousePosition = currentMousePosition;
}

void MultiDrawApp::Update(float deltaSeconds)
{
    // 뷰 행렬 구축
    const XMVECTOR cameraPosition = Math::SphericalToCartesian(radius, theta, phi);
    const XMVECTOR targetPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    const XMMATRIX newViewMatrix = XMMatrixLookAtLH(cameraPosition, targetPosition, upDirection);
    XMStoreFloat4x4(&viewMatrix, newViewMatrix);
}

void MultiDrawApp::Render()
{
    Super::Render();

    if (!immediateContext || !renderTargetView || !depthStencilView || !inputLayout || !vertexBuffer || !indexBuffer || !constantBuffer || !vertexShader || !pixelShader)
    {
        return;
    }

    // 클리어
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), Colors::Blue);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // 와이어 프레임으로 보기
    immediateContext->RSSetState(wireframeRasterizerState.Get());

    // 각 오브젝트에 맞는 wvp행렬을 적용하여 그리기
    const XMMATRIX vpMatrix = XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);
    auto drawObject = [&](const XMFLOAT4X4& worldMatrix, UINT indexCount, UINT startIndexCount, INT baseVertexCount)
    {
        // constant buffer 업데이트
        XMFLOAT4X4 wvpMatrix;
        XMStoreFloat4x4(&wvpMatrix, XMLoadFloat4x4(&worldMatrix) * vpMatrix);
        immediateContext->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &wvpMatrix, 0, 0);

        // 셰이더 호출
        immediateContext->DrawIndexed(indexCount, startIndexCount, baseVertexCount);
    };

    drawObject(boxWorldMatrix, boxSubmesh.indexCount, boxSubmesh.startIndexLocation, boxSubmesh.baseVertexLocation);
    drawObject(gridWorldMatrix, gridSubmesh.indexCount, gridSubmesh.startIndexLocation, gridSubmesh.baseVertexLocation);
    drawObject(centerSphereWorldMatrix, sphereSubmesh.indexCount, sphereSubmesh.startIndexLocation, sphereSubmesh.baseVertexLocation);

    for (const auto& sphereWorldMatrix : sphereWorldMatrices)
    {
        drawObject(sphereWorldMatrix, sphereSubmesh.indexCount, sphereSubmesh.startIndexLocation, sphereSubmesh.baseVertexLocation);
    }

    for (const XMFLOAT4X4& cylinderWorldMatrix : cylinderWorldMatrices)
    {
        drawObject(cylinderWorldMatrix, cylinderSubmesh.indexCount, cylinderSubmesh.startIndexLocation, cylinderSubmesh.baseVertexLocation);
    }

    swapChain->Present(0, 0);
}

void MultiDrawApp::CreateGeometryBuffers()
{
    const GeometryGenerator::MeshData box = GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f);
    const GeometryGenerator::MeshData grid = GeometryGenerator::CreateGrid(20.0f, 30.0f, 60, 40);
    const GeometryGenerator::MeshData sphere = GeometryGenerator::CreateGeodesicSphere(0.5f, 3);
    const GeometryGenerator::MeshData cylinder = GeometryGenerator::CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

    boxSubmesh = Submesh(box.indices.size(), 0, 0);
    gridSubmesh = Submesh(grid.indices.size(), box.indices.size(), box.vertices.size());
    sphereSubmesh = Submesh(sphere.indices.size(), gridSubmesh.startIndexLocation + grid.indices.size(), gridSubmesh.baseVertexLocation + grid.vertices.size());
    cylinderSubmesh = Submesh(cylinder.indices.size(), sphereSubmesh.startIndexLocation + sphere.indices.size(), sphereSubmesh.baseVertexLocation + sphere.vertices.size());

    std::vector<Vertex> vertices(box.vertices.size() + grid.vertices.size() + sphere.vertices.size() + cylinder.vertices.size());

    UINT currentVertexIndex = 0;
    const XMFLOAT4 black(Colors::Black);
    auto appendVertex = [&](const std::vector<GeometryGenerator::Vertex>& inVertices)
    {
        for (size_t i = 0; i < inVertices.size(); ++i, ++currentVertexIndex)
        {
            vertices[currentVertexIndex].pos = inVertices[i].position;
            vertices[currentVertexIndex].color = black;
        }
    };

    appendVertex(box.vertices);
    appendVertex(grid.vertices);
    appendVertex(sphere.vertices);
    appendVertex(cylinder.vertices);

    std::vector<UINT> indices;
    indices.reserve(box.indices.size() + grid.indices.size() + sphere.indices.size() + cylinder.indices.size());
    indices.insert(indices.end(), box.indices.begin(), box.indices.end());
    indices.insert(indices.end(), grid.indices.begin(), grid.indices.end());
    indices.insert(indices.end(), sphere.indices.begin(), sphere.indices.end());
    indices.insert(indices.end(), cylinder.indices.begin(), cylinder.indices.end());

    // 버텍스 버퍼 생성
    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{vertices.data()};
    CHECK_HR(device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &vertexBuffer), L"버텍스 버퍼 생성에 실패했습니다.");

    // 인덱스 버퍼 생성
    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * indices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{indices.data()};
    CHECK_HR(device->CreateBuffer(&indexBufferDesc, &indexInitData, &indexBuffer), L"인덱스 버퍼 생성에 실패했습니다.");

    // 인덱스 카운트 업데이트
    indexCount = static_cast<UINT>(indices.size());
}

void MultiDrawApp::CreateShaders()
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
    CHECK_HR(device->CreateInputLayout(vertexDesc.data(), static_cast<UINT>(vertexDesc.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout), L"인풋 레이아웃 생성 실패");

    // 상수버퍼 생성
    const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(XMMATRIX), D3D11_BIND_CONSTANT_BUFFER);
    device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
}

void MultiDrawApp::BindShader()
{
    if (!immediateContext || !inputLayout || !vertexBuffer || !indexBuffer || !constantBuffer || !vertexShader || !pixelShader)
    {
        return;
    }

    // input assembler 연결
    immediateContext->IASetInputLayout(inputLayout.Get());
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    constexpr UINT stride = sizeof(Vertex);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // constant buffer 연결
    immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    // shader 연결
    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
}
