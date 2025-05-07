#include "MirrorDemoApp.h"

#include <numbers>
#include <span>
#include <External/DirectXTex/DirectXTex.h>

#include "Core/Common/GeometryGenerator.h"
#include "Core/Common/Timer.h"
#include "Core/Data/Color.h"
#include "Core/Data/Path.h"
#include "Core/Data/SphericalCoord.h"
#include "Core/Rendering/Vertex.h"
#include "Core/Utilities/Utility.h"
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

    constexpr float Zero4[4] = {0.0f, 0.0f, 0.0f, 0.0f};
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
    SetCameraSphericalCoord(75.f, XMConvertToRadians(-70.0f), XMConvertToRadians(75.0f));
    SetZoomSpeed(0.25f);

    const XMMATRIX identityMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&hillsUVMatrix, XMMatrixScaling(5.0f, 5.0f, 0.0f));
    XMStoreFloat4x4(&hillsWorldMatrix, identityMatrix);
    XMStoreFloat4x4(&wavesWorldMatrix, identityMatrix);
    XMStoreFloat4x4(&wavesWorldMatrix, identityMatrix);
    XMStoreFloat4x4(&wireFenceWorldMatrix, XMMatrixScalingFromVector(XMVectorReplicate(15.0f)) * XMMatrixTranslation(8.0f, 5.0f, -15.0f));
    XMStoreFloat4x4(&wireFenceUVMatrix, identityMatrix);

    hillsMaterial.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    hillsMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    hillsMaterial.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

    wavesMaterial.ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    wavesMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.75f);
    wavesMaterial.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

    wireFenceMaterial.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    wireFenceMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    wireFenceMaterial.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
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

    static float elapsedTime = 0.0f;
    if (timer->GetTotalSeconds() - elapsedTime >= 0.1f)
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

    Vertex::PNT* waveVertices = static_cast<Vertex::PNT*>(mapped.pData);
    for (UINT i = 0; i < waves.VertexCount(); ++i)
    {
        waveVertices[i].position = waves[i];
        waveVertices[i].normal = waves.Normal(i);
        waveVertices[i].tex.x = 0.5f + waves[i].x / waves.Width();
        waveVertices[i].tex.y = 0.5f - waves[i].z / waves.Depth();
    }

    immediateContext->Unmap(wavesVertexBuffer.Get(), 0);

    XMStoreFloat4x4(&wavesUVMatrix, XMMatrixScaling(5.0f, 5.0f, 0.0f) + XMMatrixTranslation(static_cast<float>(timer->GetTotalSeconds() * 0.05f), static_cast<float>(timer->GetTotalSeconds() * 0.1f), 0.0f));
}

void MirrorDemoApp::Render()
{
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), LinearColors::LightSteelBlue);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    const XMMATRIX viewProjectionMatrix = XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);
    RenderObject(
        hillsVertexBuffer.Get(), hillsIndexBuffer.Get(), nullptr, nullptr,
        XMLoadFloat4x4(&hillsWorldMatrix), viewProjectionMatrix, XMLoadFloat4x4(&hillsUVMatrix),
        hillsDiffuseMapSRV.Get(), hillsMaterial, hillsSubmesh.indexCount
    );
    RenderObject(
        wireFenceVertexBuffer.Get(), wireFenceIndexBuffer.Get(), noCullRasterizerState.Get(), alphaToCoverageBlendState.Get(),
        XMLoadFloat4x4(&wireFenceWorldMatrix), viewProjectionMatrix, XMLoadFloat4x4(&wireFenceUVMatrix),
        wireFenceDiffuseMapSRV.Get(), wireFenceMaterial, wireFenceSubmesh.indexCount
    );
    RenderObject(
        wavesVertexBuffer.Get(), wavesIndexBuffer.Get(), nullptr, transparentBlendState.Get(),
        XMLoadFloat4x4(&wavesWorldMatrix), viewProjectionMatrix, XMLoadFloat4x4(&wavesUVMatrix),
        wavesDiffuseMapSRV.Get(), wavesMaterial, wavesSubmesh.indexCount
    );
    swapChain->Present(0, 0);
}

void MirrorDemoApp::RenderObject(
    ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr, ID3D11RasterizerState* rasterizerState, ID3D11BlendState* blendState,
    FXMMATRIX worldMatrix, CXMMATRIX viewProjectionMatrix, CXMMATRIX uvMatrix,
    ID3D11ShaderResourceView* diffuseMapSRV, const Material& material, UINT indexCount
)
{
    shaderPass->SetMatrix(worldMatrix, viewProjectionMatrix);
    shaderPass->SetUVMatrix(uvMatrix);
    shaderPass->SetMaterial(material);
    shaderPass->SetLights(directionalLights);
    shaderPass->SetEyePosition(eyePosition);
    shaderPass->SetFogColor(LinearColors::LightSteelBlue);
    shaderPass->SetFogRange(15.0f, 200.0f);
    shaderPass->UpdateCBuffer(immediateContext.Get());

    constexpr UINT stride = sizeof(Vertex::PNT);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);
    immediateContext->IASetIndexBuffer(indexBufferPtr, DXGI_FORMAT_R32_UINT, 0);
    immediateContext->PSSetShaderResources(0, 1, std::array{diffuseMapSRV}.data());

    if (IsWireframe())
    {
        immediateContext->RSSetState(wireframeRasterizerState.Get());
    }
    else
    {
        if (blendState)
        {
            immediateContext->OMSetBlendState(blendState, Zero4, 0xffffffff);
        }

        if (rasterizerState)
        {
            immediateContext->RSSetState(rasterizerState);
        }
    }

    immediateContext->DrawIndexed(indexCount, 0, 0);

    immediateContext->OMSetBlendState(nullptr, Zero4, 0xffffffff);
    immediateContext->RSSetState(nullptr);
}

void MirrorDemoApp::CreateGeometry()
{
    CreateLandGeometry();
    CreateWaveGeometry();
    CreateWireFenceGeometry();
}

void MirrorDemoApp::CreateLandGeometry()
{
    const auto [meshVertices, meshIndices] = GeometryGenerator::CreateGrid(160.0f, 160.0f, 50, 50);
    hillsSubmesh = Submesh(meshIndices.size(), 0, 0);

    std::vector<Vertex::PNT> vertices(meshVertices.size());
    std::ranges::transform(meshVertices, vertices.begin(), [](const GeometryGenerator::Vertex& vertex)
    {
        return Vertex::PNT
        {
            .position = XMFLOAT3(vertex.position.x, GetHeight(vertex.position.x, vertex.position.z), vertex.position.z),
            .normal = GetHillNormal(vertex.position.x, vertex.position.z),
            .tex = vertex.texC
        };
    });

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex::PNT) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{vertices.data()};
    device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &hillsVertexBuffer);

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * hillsSubmesh.indexCount), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{meshIndices.data()};
    device->CreateBuffer(&indexBufferDesc, &indexInitData, &hillsIndexBuffer);
}

void MirrorDemoApp::CreateWaveGeometry()
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

    const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(Vertex::PNT) * waves.VertexCount(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&vertexBufferDesc, nullptr, &wavesVertexBuffer);

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * indices.size()), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{indices.data()};
    device->CreateBuffer(&indexBufferDesc, &indexInitData, &wavesIndexBuffer);
}

void MirrorDemoApp::CreateWireFenceGeometry()
{
    const auto [meshVertices, meshIndices] = GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f);
    wireFenceSubmesh = Submesh(meshIndices.size(), 0, 0);

    std::vector<Vertex::PNT> vertices(meshVertices.size());
    std::ranges::transform(meshVertices, vertices.begin(), [](const GeometryGenerator::Vertex& vertex)
    {
        return Vertex::PNT
        {
            .position = vertex.position,
            .normal = vertex.normal,
            .tex = vertex.texC
        };
    });

    const CD3D11_BUFFER_DESC vertexBufferDesc(static_cast<UINT>(sizeof(Vertex::PNT) * vertices.size()), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA vertexInitData{vertices.data()};
    device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &wireFenceVertexBuffer);

    const CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(UINT) * wireFenceSubmesh.indexCount), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    const D3D11_SUBRESOURCE_DATA indexInitData{meshIndices.data()};
    device->CreateBuffer(&indexBufferDesc, &indexInitData, &wireFenceIndexBuffer);
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

    LoadFromDDSFile(Path::GetTexturePath(L"water2.dds").c_str(), DDS_FLAGS_NONE, &metaData, image);
    CreateShaderResourceView(device.Get(), image.GetImages(), image.GetImageCount(), metaData, &wavesDiffuseMapSRV);

    LoadFromDDSFile(Path::GetTexturePath(L"grass.dds").c_str(), DDS_FLAGS_NONE, &metaData, image);
    CreateShaderResourceView(device.Get(), image.GetImages(), image.GetImageCount(), metaData, &hillsDiffuseMapSRV);

    LoadFromDDSFile(Path::GetTexturePath(L"WireFence.dds").c_str(), DDS_FLAGS_NONE, &metaData, image);
    CreateShaderResourceView(device.Get(), image.GetImages(), image.GetImageCount(), metaData, &wireFenceDiffuseMapSRV);
}
