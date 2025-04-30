#include "CrateShaderPass.h"

#include <array>

#include "Core/Data/Color.h"
#include "Core/Rendering/Vertex.h"

CrateShaderPass::CrateShaderPass(ID3D11Device* device)
    : ShaderPassBase(device, L"CraftPass_vs", L"CraftPass_ps", Vertex::PNT::Desc)
{
    const CD3D11_BUFFER_DESC renderDateCBufferDesc(sizeof(RenderData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&renderDateCBufferDesc, nullptr, &renderDataCBuffer);

    const CD3D11_BUFFER_DESC lightDataCBufferDesc(sizeof(LightData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&lightDataCBufferDesc, nullptr, &lightDataCBuffer);

    const CD3D11_SAMPLER_DESC samplerDesc(
        D3D11_FILTER_ANISOTROPIC,
        D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
        0.0f, 8,
        D3D11_COMPARISON_NEVER,
        LinearColors::Black,
        0.0f, D3D11_FLOAT32_MAX
    );
    device->CreateSamplerState(&samplerDesc, &samplerState);
}

CrateShaderPass::~CrateShaderPass() = default;

void CrateShaderPass::Bind(ID3D11DeviceContext* context)
{
    Super::Bind(context);

    ID3D11Buffer* cBuffers[] = {renderDataCBuffer.Get(), lightDataCBuffer.Get()};
    context->VSSetConstantBuffers(0, 2, cBuffers);
    context->PSSetConstantBuffers(0, 2, cBuffers);
    context->PSSetSamplers(0, 1, std::array{samplerState.Get()}.data());
}

void CrateShaderPass::UpdateCBuffer(ID3D11DeviceContext* immediateContext)
{
    UpdateRenderData(immediateContext);
    UpdateLightData(immediateContext);
}

void CrateShaderPass::SetMatrix(DirectX::FXMMATRIX worldMatrix, DirectX::CXMMATRIX viewMatrix, DirectX::CXMMATRIX projectionMatrix)
{
    XMStoreFloat4x4(&renderData.worldMatrix, worldMatrix);
    XMStoreFloat4x4(&renderData.worldInverseTransposeMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix)));
    XMStoreFloat4x4(&renderData.wvpMatrix, worldMatrix * viewMatrix * projectionMatrix);
    renderDataDirty = true;
}

void CrateShaderPass::SetTextureTransformMatrix(const DirectX::XMFLOAT4X4& transformMatrix)
{
    renderData.textureTransformMatrix = transformMatrix;
    renderDataDirty = true;
}

void CrateShaderPass::SetMaterial(const Material& material)
{
    renderData.material = material;
    renderDataDirty = true;
}

void CrateShaderPass::SetUseTexture(bool newUseTexture)
{
    renderData.useTexture = newUseTexture;
    renderDataDirty = true;
}

void CrateShaderPass::SetDirectionalLights(const std::array<DirectionalLight, 3>& directionalLights)
{
    std::ranges::copy(directionalLights, lightData.directionalLights);
    lightDataDirty = true;
}

void CrateShaderPass::SetEyePosition(const DirectX::XMFLOAT3& eyePosition)
{
    lightData.eyePosition = eyePosition;
    lightDataDirty = true;
}

void CrateShaderPass::SetActiveDirectionalLightCount(UINT activeCount)
{
    lightData.activeCount = activeCount;
    lightDataDirty = true;
}

void CrateShaderPass::SetDiffuseMap(ID3D11DeviceContext* context, ID3D11ShaderResourceView* diffuseMapSRV)
{
    context->PSSetShaderResources(0, 1, std::array{diffuseMapSRV}.data());
}

void CrateShaderPass::UpdateRenderData(ID3D11DeviceContext* immediateContext)
{
    if (!renderDataDirty)
    {
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mapped;
    immediateContext->Map(renderDataCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    *static_cast<RenderData*>(mapped.pData) = renderData;
    immediateContext->Unmap(renderDataCBuffer.Get(), 0);

    renderDataDirty = false;
}

void CrateShaderPass::UpdateLightData(ID3D11DeviceContext* immediateContext)
{
    if (!lightDataDirty)
    {
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mapped;
    immediateContext->Map(lightDataCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    *static_cast<LightData*>(mapped.pData) = lightData;
    immediateContext->Unmap(lightDataCBuffer.Get(), 0);

    lightDataDirty = false;
}
