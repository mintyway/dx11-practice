#include "MirrorDemoShaderPass.h"

#include <span>

#include "Core/Data/Path.h"
#include "Core/Rendering/Vertex.h"

using namespace DirectX;

MirrorDemoShaderPass::MirrorDemoShaderPass(ID3D11Device* device)
    : Super(device, L"MirrorDemoShader_vs", L"MirrorDemoShader_ps", Vertex::PNT::Desc)
{
    const CD3D11_BUFFER_DESC objectRenderCBufferDesc(sizeof(RenderData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&objectRenderCBufferDesc, nullptr, &renderCBuffer);

    const CD3D11_BUFFER_DESC sceneLightDataCBufferDesc(sizeof(LightData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&sceneLightDataCBufferDesc, nullptr, &lightCBuffer);
}

void MirrorDemoShaderPass::Bind(ID3D11DeviceContext* context)
{
    Super::Bind(context);

    ID3D11Buffer* cBuffers[] = {renderCBuffer.Get(), lightCBuffer.Get()};
    context->VSSetConstantBuffers(0, 2, cBuffers);
    context->PSSetConstantBuffers(0, 2, cBuffers);
}

void MirrorDemoShaderPass::UpdateCBuffer(ID3D11DeviceContext* context)
{
    UpdateRenderData(context);
    UpdateLightData(context);
}

void MirrorDemoShaderPass::SetMatrix(DirectX::FXMMATRIX worldMatrix, DirectX::CXMMATRIX viewProjectionMatrix)
{
    XMStoreFloat4x4(&renderData.worldMatrix, worldMatrix);
    XMStoreFloat4x4(&renderData.worldInverseTransposeMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix)));
    XMStoreFloat4x4(&renderData.wvpMatrix, worldMatrix * viewProjectionMatrix);
}

void MirrorDemoShaderPass::SetUVMatrix(DirectX::FXMMATRIX uvMatrix)
{
    XMStoreFloat4x4(&renderData.uvMatrix, uvMatrix);
}

void MirrorDemoShaderPass::SetMaterial(const Material& material)
{
    renderData.material = material;
}

void MirrorDemoShaderPass::SetLights(std::span<const DirectionalLight, 3> directionalLights)
{
    std::ranges::copy(directionalLights, lightData.directionalLight);
}

void MirrorDemoShaderPass::SetEyePosition(const DirectX::XMFLOAT3& eyePosition)
{
    lightData.eyePosition = eyePosition;
}

void MirrorDemoShaderPass::SetActiveDirectionalLightCount(UINT count)
{
    lightData.activeDirectionalLightCount = count;
}

void MirrorDemoShaderPass::SetFogColor(DirectX::FXMVECTOR color)
{
    XMStoreFloat4(&lightData.fogColor, color);
}

void MirrorDemoShaderPass::SetFogRange(float start, float end)
{
    lightData.fogStart = start;
    lightData.fogRange = std::max(end - start, 0.0f);
}

void MirrorDemoShaderPass::UpdateRenderData(ID3D11DeviceContext* context)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    context->Map(renderCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    *static_cast<RenderData*>(mapped.pData) = renderData;
    context->Unmap(renderCBuffer.Get(), 0);
}

void MirrorDemoShaderPass::UpdateLightData(ID3D11DeviceContext* context)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    context->Map(lightCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    *static_cast<LightData*>(mapped.pData) = lightData;
    context->Unmap(lightCBuffer.Get(), 0);
}
