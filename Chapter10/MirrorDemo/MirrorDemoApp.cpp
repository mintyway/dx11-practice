#include "MirrorDemoApp.h"

#include <fstream>
#include <numbers>
#include <span>
#include <External/DirectXTex/DirectXTex.h>

#include "Core/Common/GeometryGenerator.h"
#include "Core/Data/Color.h"
#include "Core/Data/Path.h"
#include "Core/Data/SphericalCoord.h"
#include "Core/Rendering/Vertex.h"
#include "Shaders/MirrorDemoShaderPass.h"

using namespace DirectX;

namespace
{
    constexpr float InvSqrt3 = std::numbers::inv_sqrt3_v<float>;
    constexpr float InvSqrt2 = 1.0f / std::numbers::sqrt2_v<float>;

    constexpr std::array DefaultDirectionalLights =
    {
        DirectionalLight
        {
            .ambient = {0.2f, 0.2f, 0.2f, 1.0f},
            .diffuse = {0.5f, 0.5f, 0.5f, 1.0f},
            .specular = {0.5f, 0.5f, 0.5f, 1.0f},
            .direction = {InvSqrt3, -InvSqrt3, InvSqrt3}
        },
        DirectionalLight
        {
            .ambient = {0.0f, 0.0f, 0.0f, 1.0f},
            .diffuse = {0.2f, 0.2f, 0.2f, 1.0f},
            .specular = {0.25f, 0.25f, 0.25f, 1.0f},
            .direction = {-InvSqrt3, -InvSqrt3, InvSqrt3}
        },
        DirectionalLight
        {
            .ambient = {0.0f, 0.0f, 0.0f, 1.0f},
            .diffuse = {0.2f, 0.2f, 0.2f, 1.0f},
            .specular = {0.0f, 0.0f, 0.0f, 1.0f},
            .direction = {0.0f, -InvSqrt2, -InvSqrt2}
        }
    };
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    const std::unique_ptr<EngineBase> engine = std::make_unique<MirrorDemoApp>();
    if (engine->Init(hInstance))
    {
        engine->Run();
    }

    return 0;
}

MirrorDemoApp::MirrorDemoApp()
    : directionalLights(DefaultDirectionalLights)
{
    SetCameraSphericalCoord(25.f, XMConvertToRadians(-70.0f), XMConvertToRadians(75.0f));
    SetZoomSpeed(0.05f);

    const XMMATRIX identityMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&skullWorldMatrix, identityMatrix);
    XMStoreFloat4x4(&roomWorldMatrix, identityMatrix);

    skullTranslation = XMFLOAT3(0.0f, 1.0f, -5.0f);

    skullMaterial.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    skullMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    skullMaterial.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

    roomMaterial.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    roomMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    roomMaterial.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

    mirrorMaterial.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mirrorMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f);
    mirrorMaterial.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
}

MirrorDemoApp::~MirrorDemoApp() = default;

bool MirrorDemoApp::Init(HINSTANCE inInstanceHandle)
{
    if (!Super::Init(inInstanceHandle))
    {
        return false;
    }

    shaderPass = std::make_unique<MirrorDemoShaderPass>(device.Get());
    shaderPass->Bind(immediateContext.Get());
    CreateGeometry();
    InitTexture();

    return true;
}

void MirrorDemoApp::OnResize()
{
    Super::OnResize();

    XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(0.25f * XM_PI, GetAspectRatio(), 1.0f, 1000.0f));
}

void MirrorDemoApp::Update(float deltaSeconds)
{
    const XMVECTOR cameraPosition = SphericalCoord::SphericalToCartesian(GetCameraSphericalCoord());
    const XMVECTOR focusPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(cameraPosition, focusPosition, upDirection));
    XMStoreFloat3(&eyePosition, cameraPosition);

    if (GetAsyncKeyState('0') & 0x8000)
    {
        shaderPass->SetActiveDirectionalLightCount(0);
        shaderPass->SetUseTexture(false);
        shaderPass->SetUseFog(false);
    }
    if (GetAsyncKeyState('1') & 0x8000)
    {
        shaderPass->SetActiveDirectionalLightCount(3);
        shaderPass->SetUseTexture(false);
        shaderPass->SetUseFog(false);
    }
    if (GetAsyncKeyState('2') & 0x8000)
    {
        shaderPass->SetActiveDirectionalLightCount(3);
        shaderPass->SetUseTexture(true);
        shaderPass->SetUseFog(false);
    }
    if (GetAsyncKeyState('3') & 0x8000)
    {
        shaderPass->SetActiveDirectionalLightCount(3);
        shaderPass->SetUseTexture(true);
        shaderPass->SetUseFog(true);
    }

    if (GetAsyncKeyState('A') & 0x8000)
    {
        skullTranslation.x -= 1.0f * deltaSeconds;
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        skullTranslation.x += 1.0f * deltaSeconds;
    }
    if (GetAsyncKeyState('W') & 0x8000)
    {
        skullTranslation.y += 1.0f * deltaSeconds;
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        skullTranslation.y -= 1.0f * deltaSeconds;
    }

    skullTranslation.y = std::max(skullTranslation.y, 0.0f);

    XMStoreFloat4x4(&skullWorldMatrix, XMMatrixScaling(0.45f, 0.45f, 0.45f) * XMMatrixRotationY(XM_PIDIV2) * XMMatrixTranslationFromVector(XMLoadFloat3(&skullTranslation)));
}

void MirrorDemoApp::Render()
{
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), LinearColors::LightSteelBlue);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    const XMMATRIX viewProjectionMatrix = XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);

    shaderPass->SetLights(directionalLights);
    shaderPass->SetEyePosition(eyePosition);
    shaderPass->SetFogColor(LinearColors::LightSteelBlue);
    shaderPass->SetFogRange(15.0f, 200.0f);

    // 해골 렌더링
    shaderPass->SetMatrix(XMLoadFloat4x4(&skullWorldMatrix), viewProjectionMatrix);
    shaderPass->SetMaterial(skullMaterial);
    shaderPass->SetUseTexture(false);
    shaderPass->UpdateCBuffer(immediateContext.Get());

    immediateContext->IASetVertexBuffers(0, 1, skullVertexBuffer.GetAddressOf(), std::array{static_cast<UINT>(sizeof(Vertex::PNT))}.data(), std::array{0u}.data());
    immediateContext->IASetIndexBuffer(skullIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediateContext->DrawIndexed(skullSubmesh.indexCount, skullSubmesh.startIndexLocation, skullSubmesh.baseVertexLocation);

    // 방 렌더링
    shaderPass->SetMatrix(XMLoadFloat4x4(&roomWorldMatrix), viewProjectionMatrix);
    shaderPass->SetUVMatrix(XMMatrixIdentity());
    shaderPass->SetMaterial(roomMaterial);
    shaderPass->SetUseTexture(true);
    shaderPass->UpdateCBuffer(immediateContext.Get());

    immediateContext->IASetVertexBuffers(0, 1, roomVertexBuffer.GetAddressOf(), std::array{static_cast<UINT>(sizeof(Vertex::PNT))}.data(), std::array{0u}.data());

    immediateContext->PSSetShaderResources(0, 1, std::array{floorDiffuseMapSrv.Get()}.data());
    immediateContext->Draw(6, 0);

    immediateContext->PSSetShaderResources(0, 1, std::array{wallDiffuseMapSrv.Get()}.data());
    immediateContext->Draw(18, 6);

    // 거울을 스텐실 버퍼에 기록
    shaderPass->SetMatrix(XMLoadFloat4x4(&roomWorldMatrix), viewProjectionMatrix);
    shaderPass->SetUseTexture(true);
    shaderPass->UpdateCBuffer(immediateContext.Get());

    immediateContext->IASetVertexBuffers(0, 1, roomVertexBuffer.GetAddressOf(), std::array{static_cast<UINT>(sizeof(Vertex::PNT))}.data(), std::array{0u}.data());
    immediateContext->OMSetDepthStencilState(markMirrorDepthStencilState.Get(), 1);
    immediateContext->OMSetBlendState(noRenderTargetWriteBlendState.Get(), nullptr, 0xffffffff);
    immediateContext->Draw(6, 24);

    immediateContext->OMSetDepthStencilState(nullptr, 0);
    immediateContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

    // 반사된 해골 렌더링
    const XMMATRIX reflectionMatrix = XMMatrixReflect(XMVECTORF32{0.0f, 0.0f, 1.0f, 0.0f});

    shaderPass->SetMatrix(XMLoadFloat4x4(&skullWorldMatrix) * reflectionMatrix, viewProjectionMatrix);
    shaderPass->SetUseTexture(false);
    shaderPass->SetMaterial(skullMaterial);

    std::array<DirectionalLight, 3> reflectedDirectionalLights = directionalLights;
    for (int i = 0; i < reflectedDirectionalLights.size(); ++i)
    {
        const XMVECTOR reflectedLightDirection = XMVector3TransformNormal(XMLoadFloat3(&reflectedDirectionalLights[i].direction), reflectionMatrix);
        XMStoreFloat3(&reflectedDirectionalLights[i].direction, reflectedLightDirection);
    }
    shaderPass->SetLights(reflectedDirectionalLights);

    shaderPass->UpdateCBuffer(immediateContext.Get());

    immediateContext->IASetVertexBuffers(0, 1, skullVertexBuffer.GetAddressOf(), std::array{static_cast<UINT>(sizeof(Vertex::PNT))}.data(), std::array{0u}.data());
    immediateContext->IASetIndexBuffer(skullIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediateContext->RSSetState(counterClockwiseRasterizerState.Get());
    immediateContext->OMSetDepthStencilState(renderReflectionDepthStencilState.Get(), 1);
    immediateContext->DrawIndexed(skullSubmesh.indexCount, skullSubmesh.startIndexLocation, skullSubmesh.baseVertexLocation);

    shaderPass->SetLights(directionalLights);
    immediateContext->RSSetState(nullptr);
    immediateContext->OMSetDepthStencilState(nullptr, 0);

    // 반사된 방 렌더링
    shaderPass->SetMatrix(XMLoadFloat4x4(&roomWorldMatrix) * reflectionMatrix, viewProjectionMatrix);
    shaderPass->SetUVMatrix(XMMatrixIdentity());
    shaderPass->SetMaterial(roomMaterial);
    shaderPass->SetLights(reflectedDirectionalLights);
    shaderPass->SetUseTexture(true);
    shaderPass->UpdateCBuffer(immediateContext.Get());

    immediateContext->IASetVertexBuffers(0, 1, roomVertexBuffer.GetAddressOf(), std::array{static_cast<UINT>(sizeof(Vertex::PNT))}.data(), std::array{0u}.data());
    immediateContext->RSSetState(counterClockwiseRasterizerState.Get());
    immediateContext->OMSetDepthStencilState(renderReflectionDepthStencilState.Get(), 1);

    immediateContext->PSSetShaderResources(0, 1, std::array{floorDiffuseMapSrv.Get()}.data());
    immediateContext->Draw(6, 0);

    immediateContext->PSSetShaderResources(0, 1, std::array{wallDiffuseMapSrv.Get()}.data());
    immediateContext->Draw(18, 6);

    shaderPass->SetLights(directionalLights);
    immediateContext->RSSetState(nullptr);
    immediateContext->OMSetDepthStencilState(nullptr, 0);

    // 거울 머티리얼 렌더링
    shaderPass->SetMatrix(XMLoadFloat4x4(&roomWorldMatrix), viewProjectionMatrix);
    shaderPass->SetUVMatrix(XMMatrixIdentity());
    shaderPass->SetMaterial(mirrorMaterial);
    shaderPass->SetUseTexture(true);
    shaderPass->UpdateCBuffer(immediateContext.Get());

    immediateContext->IASetVertexBuffers(0, 1, roomVertexBuffer.GetAddressOf(), std::array{static_cast<UINT>(sizeof(Vertex::PNT))}.data(), std::array{0u}.data());
    immediateContext->PSSetShaderResources(0, 1, std::array{mirrorDiffuseMapSrv.Get()}.data());
    immediateContext->OMSetBlendState(transparentBlendState.Get(), nullptr, 0xffffffff);
    immediateContext->Draw(6, 24);

    immediateContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

    swapChain->Present(0, 0);
}

void MirrorDemoApp::RenderObject(
    ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr,
    FXMMATRIX worldMatrix, CXMMATRIX viewProjectionMatrix, CXMMATRIX uvMatrix,
    ID3D11ShaderResourceView* diffuseMapSRV, const Material& material, Submesh submesh
)
{
    shaderPass->SetMatrix(worldMatrix, viewProjectionMatrix);
    shaderPass->SetUVMatrix(uvMatrix);
    shaderPass->SetMaterial(material);
    shaderPass->UpdateCBuffer(immediateContext.Get());

    constexpr UINT stride = sizeof(Vertex::PNT);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);
    immediateContext->IASetIndexBuffer(indexBufferPtr, DXGI_FORMAT_R32_UINT, 0);
    immediateContext->PSSetShaderResources(0, 1, std::array{diffuseMapSRV}.data());

    immediateContext->DrawIndexed(submesh.indexCount, submesh.startIndexLocation, submesh.baseVertexLocation);
}

void MirrorDemoApp::CreateGeometry()
{
    CreateSkullGeometry();
    CreateRoomGeometry();
}

void MirrorDemoApp::CreateSkullGeometry()
{
    std::ifstream ifs(Path::GetModelPath(L"skull.txt").c_str());

    UINT vertexCount = 0;
    UINT triangleCount = 0;
    std::string ignore;

    ifs >> ignore >> vertexCount;
    ifs >> ignore >> triangleCount;
    ifs >> ignore >> ignore >> ignore >> ignore;

    std::vector<Vertex::PNT> vertices(vertexCount);
    for (UINT i = 0; i < vertexCount; ++i)
    {
        ifs >> vertices[i].position.x >> vertices[i].position.y >> vertices[i].position.z;
        ifs >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;
    }

    ifs >> ignore >> ignore >> ignore;

    std::vector<UINT> indices(triangleCount * 3);
    for (UINT i = 0; i < triangleCount; ++i)
    {
        const UINT currentIndex = i * 3;
        ifs >> indices[currentIndex] >> indices[currentIndex + 1] >> indices[currentIndex + 2];
    }

    skullSubmesh = Submesh(indices.size(), 0, 0);

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex::PNT) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{.pSysMem = vertices.data()};
    device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &skullVertexBuffer);

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * indices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{.pSysMem = indices.data()};
    device->CreateBuffer(&indexBufferDesc, &indexInitData, &skullIndexBuffer);
}

void MirrorDemoApp::CreateRoomGeometry()
{
    std::array<Vertex::PNT, 30> vertices;

    // 바닥: 텍스처 좌표를 통해 타일링 적용
    vertices[0] = Vertex::PNT{.position = XMFLOAT3(-3.5f, 0.0f, -10.0f), .normal = XMFLOAT3(0.0f, 1.0f, 0.0f), .tex = XMFLOAT2(0.0f, 4.0f)};
    vertices[1] = Vertex::PNT{.position = XMFLOAT3(-3.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 1.0f, 0.0f), .tex = XMFLOAT2(0.0f, 0.0f)};
    vertices[2] = Vertex::PNT{.position = XMFLOAT3(7.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 1.0f, 0.0f), .tex = XMFLOAT2(4.0f, 0.0f)};

    vertices[3] = Vertex::PNT{.position = XMFLOAT3(-3.5f, 0.0f, -10.0f), .normal = XMFLOAT3(0.0f, 1.0f, 0.0f), .tex = XMFLOAT2(0.0f, 4.0f)};
    vertices[4] = Vertex::PNT{.position = XMFLOAT3(7.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 1.0f, 0.0f), .tex = XMFLOAT2(4.0f, 0.0f)};
    vertices[5] = Vertex::PNT{.position = XMFLOAT3(7.5f, 0.0f, -10.0f), .normal = XMFLOAT3(0.0f, 1.0f, 0.0f), .tex = XMFLOAT2(4.0f, 4.0f)};

    // 벽: 텍스처 좌표를 통한 타일링
    vertices[6] = Vertex::PNT{.position = XMFLOAT3(-3.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 2.0f)};
    vertices[7] = Vertex::PNT{.position = XMFLOAT3(-3.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 0.0f)};
    vertices[8] = Vertex::PNT{.position = XMFLOAT3(-2.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.5f, 0.0f)};

    vertices[9] = Vertex::PNT{.position = XMFLOAT3(-3.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 2.0f)};
    vertices[10] = Vertex::PNT{.position = XMFLOAT3(-2.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.5f, 0.0f)};
    vertices[11] = Vertex::PNT{.position = XMFLOAT3(-2.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.5f, 2.0f)};

    vertices[12] = Vertex::PNT{.position = XMFLOAT3(2.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 2.0f)};
    vertices[13] = Vertex::PNT{.position = XMFLOAT3(2.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 0.0f)};
    vertices[14] = Vertex::PNT{.position = XMFLOAT3(7.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(2.0f, 0.0f)};

    vertices[15] = Vertex::PNT{.position = XMFLOAT3(2.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 2.0f)};
    vertices[16] = Vertex::PNT{.position = XMFLOAT3(7.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(2.0f, 0.0f)};
    vertices[17] = Vertex::PNT{.position = XMFLOAT3(7.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(2.0f, 2.0f)};

    vertices[18] = Vertex::PNT{.position = XMFLOAT3(-3.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 1.0f)};
    vertices[19] = Vertex::PNT{.position = XMFLOAT3(-3.5f, 6.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 0.0f)};
    vertices[20] = Vertex::PNT{.position = XMFLOAT3(7.5f, 6.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(6.0f, 0.0f)};

    vertices[21] = Vertex::PNT{.position = XMFLOAT3(-3.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 1.0f)};
    vertices[22] = Vertex::PNT{.position = XMFLOAT3(7.5f, 6.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(6.0f, 0.0f)};
    vertices[23] = Vertex::PNT{.position = XMFLOAT3(7.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(6.0f, 1.0f)};

    // 거울
    vertices[24] = Vertex::PNT{.position = XMFLOAT3(-2.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 1.0f)};
    vertices[25] = Vertex::PNT{.position = XMFLOAT3(-2.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 0.0f)};
    vertices[26] = Vertex::PNT{.position = XMFLOAT3(2.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(1.0f, 0.0f)};

    vertices[27] = Vertex::PNT{.position = XMFLOAT3(-2.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(0.0f, 1.0f)};
    vertices[28] = Vertex::PNT{.position = XMFLOAT3(2.5f, 4.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(1.0f, 0.0f)};
    vertices[29] = Vertex::PNT{.position = XMFLOAT3(2.5f, 0.0f, 0.0f), .normal = XMFLOAT3(0.0f, 0.0f, -1.0f), .tex = XMFLOAT2(1.0f, 1.0f)};

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex::PNT) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{.pSysMem = vertices.data()};
    device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &roomVertexBuffer);
}

void MirrorDemoApp::InitTexture()
{
    const CD3D11_SAMPLER_DESC samplerDesc(
        D3D11_FILTER_ANISOTROPIC,
        D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
        0.0f, 8,
        D3D11_COMPARISON_NEVER,
        LinearColors::Black,
        0.0f, FLT_MAX
    );
    device->CreateSamplerState(&samplerDesc, &samplerState);
    immediateContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

    ScratchImage image;
    TexMetadata metaData;

    LoadFromDDSFile(Path::GetTexturePath(L"checkboard.dds").c_str(), DDS_FLAGS_NONE, &metaData, image);
    CreateShaderResourceView(device.Get(), image.GetImages(), image.GetImageCount(), metaData, &floorDiffuseMapSrv);

    LoadFromDDSFile(Path::GetTexturePath(L"ice.dds").c_str(), DDS_FLAGS_NONE, &metaData, image);
    CreateShaderResourceView(device.Get(), image.GetImages(), image.GetImageCount(), metaData, &mirrorDiffuseMapSrv);

    LoadFromDDSFile(Path::GetTexturePath(L"brick01.dds").c_str(), DDS_FLAGS_NONE, &metaData, image);
    CreateShaderResourceView(device.Get(), image.GetImages(), image.GetImageCount(), metaData, &wallDiffuseMapSrv);
}
