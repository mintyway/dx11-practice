#include "ShaderPass.h"

#include <array>

#include "Core/Rendering/Vertex.h"

using namespace DirectX;

ShaderPass::ShaderPass(ID3D11Device* device)
    : ShaderPassBase(device, L"Chapter7LitSkullPass_vs", L"Chapter7LitSkullPass_ps", Vertex::PN::Desc)
{
    const CD3D11_BUFFER_DESC renderDateCBufferDesc(sizeof(RenderData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&renderDateCBufferDesc, nullptr, &renderDataCBuffer);

    const CD3D11_BUFFER_DESC lightDataCBufferDesc(sizeof(LightData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&lightDataCBufferDesc, nullptr, &lightDataCBuffer);
}

ShaderPass::~ShaderPass() = default;

void ShaderPass::Bind(ID3D11DeviceContext* immediateContext)
{
    ShaderPassBase::Bind(immediateContext);

    ID3D11Buffer* cBuffers[] = {renderDataCBuffer.Get(), lightDataCBuffer.Get()};
    immediateContext->VSSetConstantBuffers(0, 2, cBuffers);
    immediateContext->PSSetConstantBuffers(0, 2, cBuffers);
}

void ShaderPass::UpdateCBuffer(ID3D11DeviceContext* immediateContext)
{
    UpdateRenderData(immediateContext);
    UpdateLightData(immediateContext);
}

void ShaderPass::SetMatrix(DirectX::FXMMATRIX worldMatrix, DirectX::CXMMATRIX viewMatrix, DirectX::CXMMATRIX projectionMatrix)
{
    XMStoreFloat4x4(&renderData.worldMatrix, worldMatrix);
    XMStoreFloat4x4(&renderData.worldInverseTransposeMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix)));
    XMStoreFloat4x4(&renderData.wvpMatrix, worldMatrix * viewMatrix * projectionMatrix);
    renderDataDirty = true;
}

void ShaderPass::SetMaterial(const Material& material)
{
    renderData.material = material;
    renderDataDirty = true;
}

void ShaderPass::SetDirectionalLights(const std::array<DirectionalLight, 3>& directionalLights)
{
    std::ranges::copy(directionalLights, lightData.directionalLights);
    lightDataDirty = true;
}

void ShaderPass::SetEyePosition(const DirectX::XMFLOAT3& eyePosition)
{
    lightData.eyePosition = eyePosition;
    lightDataDirty = true;
}

void ShaderPass::SetActiveDirectionalLightCount(UINT activeCount)
{
    lightData.activeCount = activeCount;
    lightDataDirty = true;
}

void ShaderPass::UpdateRenderData(ID3D11DeviceContext* immediateContext)
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

void ShaderPass::UpdateLightData(ID3D11DeviceContext* immediateContext)
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
