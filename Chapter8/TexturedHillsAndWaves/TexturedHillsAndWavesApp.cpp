#include "TexturedHillsAndWavesApp.h"

#include <array>
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
#include "Shaders/TexturedHillsAndWavesShaderPass.h"

using namespace DirectX;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    const std::unique_ptr<EngineBase> engine = std::make_unique<TexturedHillsAndWavesApp>();
    if (engine->Init(hInstance))
    {
        engine->Run();
    }

    return 0;
}

TexturedHillsAndWavesApp::TexturedHillsAndWavesApp()
{
    SetCameraSphericalCoord(200.f, -XM_PIDIV4, XM_PIDIV4);
    SetZoomSpeed(0.25f);

    const XMMATRIX identityMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&hillsWorldMatrix, identityMatrix);
    XMStoreFloat4x4(&wavesWorldMatrix, identityMatrix);

    directionalLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    directionalLight.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    directionalLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    directionalLight.direction = XMFLOAT3(std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>);

    pointLight.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    pointLight.diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    pointLight.specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    pointLight.attenuation = XMFLOAT3(0.0f, 0.1f, 0.0f);
    pointLight.range = 25.0f;

    spotLight.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    spotLight.diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
    spotLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    spotLight.attenuation = XMFLOAT3(1.0f, 0.0f, 0.0f);
    spotLight.spot = 96.0f;
    spotLight.range = 10000.0f;

    hillsMaterial.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    hillsMaterial.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    hillsMaterial.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

    wavesMaterial.ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    wavesMaterial.diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    wavesMaterial.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

    XMStoreFloat4x4(&hillsUVMatrix, XMMatrixScaling(5.0f, 5.0f, 0.0f));
}

TexturedHillsAndWavesApp::~TexturedHillsAndWavesApp() = default;

bool TexturedHillsAndWavesApp::Init(HINSTANCE inInstanceHandle)
{
    if (!Super::Init(inInstanceHandle))
    {
        return false;
    }

    basicShader = std::make_unique<TexturedHillsAndWavesShaderPass>(device.Get());
    basicShader->Bind(immediateContext.Get());
    CreateGeometry();
    InitTexture();

    return true;
}

void TexturedHillsAndWavesApp::OnResize()
{
    Super::OnResize();

    XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(0.25f * XM_PI, GetAspectRatio(), 1.0f, 1000.0f));
}

void TexturedHillsAndWavesApp::Update(float deltaSeconds)
{
    const XMVECTOR cameraPosition = SphericalCoord::SphericalToCartesian(GetCameraSphericalCoord());
    const XMVECTOR focusPosition = XMVectorZero();
    const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(cameraPosition, focusPosition, upDirection));
    XMStoreFloat3(&eyePosition, cameraPosition);

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

    pointLight.position.x = 70.0f * std::cos(0.2f * static_cast<float>(timer->GetTotalSeconds()));
    pointLight.position.z = 70.0f * std::sin(0.2f * static_cast<float>(timer->GetTotalSeconds()));
    pointLight.position.y = std::max(GetHeight(pointLight.position.x, pointLight.position.z), -3.0f) + 10.0f;

    spotLight.position = eyePosition;
    XMStoreFloat3(&spotLight.direction, XMVector3Normalize(focusPosition - cameraPosition));

    XMStoreFloat4x4(&wavesUVMatrix, XMMatrixScaling(5.0f, 5.0f, 0.0f) + XMMatrixTranslation(static_cast<float>(timer->GetTotalSeconds() * 0.05f), static_cast<float>(timer->GetTotalSeconds() * 0.1f), 0.0f));
}

void TexturedHillsAndWavesApp::Render()
{
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), LinearColors::LightSteelBlue);
    immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    const XMMATRIX viewProjectionMatrix = XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);
    RenderObject(
        hillsVertexBuffer.Get(), hillsIndexBuffer.Get(),
        XMLoadFloat4x4(&hillsWorldMatrix), viewProjectionMatrix, XMLoadFloat4x4(&hillsUVMatrix),
        hillsDiffuseMapSRV.Get(), hillsMaterial, hillsSubmesh.indexCount
    );
    RenderObject(
        wavesVertexBuffer.Get(), wavesIndexBuffer.Get(),
        XMLoadFloat4x4(&wavesWorldMatrix), viewProjectionMatrix, XMLoadFloat4x4(&wavesUVMatrix),
        wavesDiffuseMapSRV.Get(), wavesMaterial, wavesSubmesh.indexCount
    );

    swapChain->Present(0, 0);
}

void TexturedHillsAndWavesApp::RenderObject(
    ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr,
    FXMMATRIX worldMatrix, CXMMATRIX viewProjectionMatrix, CXMMATRIX uvMatrix,
    ID3D11ShaderResourceView* diffuseMapSRV, const Material& material, UINT indexCount
)
{
    basicShader->SetMatrix(worldMatrix, viewProjectionMatrix);
    basicShader->SetUVMatrix(uvMatrix);
    basicShader->SetMaterial(material);
    basicShader->SetLights(directionalLight, pointLight, spotLight);
    basicShader->SetEyePosition(eyePosition);
    basicShader->UpdateCBuffer(immediateContext.Get());

    constexpr UINT stride = sizeof(Vertex::PNT);
    constexpr UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);
    immediateContext->IASetIndexBuffer(indexBufferPtr, DXGI_FORMAT_R32_UINT, 0);
    immediateContext->PSSetShaderResources(0, 1, std::array{diffuseMapSRV}.data());

    immediateContext->DrawIndexed(indexCount, 0, 0);
}

void TexturedHillsAndWavesApp::CreateGeometry()
{
    CreateLandGeometry();
    CreateWaveGeometry();
}

void TexturedHillsAndWavesApp::CreateLandGeometry()
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

void TexturedHillsAndWavesApp::CreateWaveGeometry()
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

void TexturedHillsAndWavesApp::InitTexture()
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
}
