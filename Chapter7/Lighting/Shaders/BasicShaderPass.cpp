#include "BasicShaderPass.h"

#include "Core/Data/Path.h"
#include "Core/Rendering/Vertex.h"

using namespace DirectX;

BasicShaderPass::BasicShaderPass(ID3D11Device* device)
    : Super(device, L"Light_vs", L"Light_ps", Vertex::PN::Desc)
{
    const CD3D11_BUFFER_DESC objectRenderCBufferDesc(sizeof(ObjectRenderData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&objectRenderCBufferDesc, nullptr, &objectRenderCBuffer);

    const CD3D11_BUFFER_DESC sceneLightDataCBufferDesc(sizeof(SceneLightData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&sceneLightDataCBufferDesc, nullptr, &sceneLightCBuffer);
}

void BasicShaderPass::Bind(ID3D11DeviceContext* immediateContext)
{
    Super::Bind(immediateContext);

    ID3D11Buffer* cBuffers[] = {objectRenderCBuffer.Get(), sceneLightCBuffer.Get()};
    immediateContext->VSSetConstantBuffers(0, 2, cBuffers);
    immediateContext->PSSetConstantBuffers(0, 2, cBuffers);
}

void BasicShaderPass::UpdateCBuffer(ID3D11DeviceContext* immediateContext)
{
    UpdateObjectRenderData(immediateContext);
    UpdateSceneLightData(immediateContext);
}

void BasicShaderPass::SetMatrix(DirectX::FXMMATRIX worldMatrix, DirectX::CXMMATRIX viewProjectionMatrix)
{
    XMStoreFloat4x4(&objectRenderData.worldMatrix, worldMatrix);
    XMStoreFloat4x4(&objectRenderData.worldInverseTransposeMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix)));
    XMStoreFloat4x4(&objectRenderData.wvpMatrix, worldMatrix * viewProjectionMatrix);
    objectRenderDataDirty = true;
}

void BasicShaderPass::SetMaterial(const Material& material)
{
    objectRenderData.material = material;
    objectRenderDataDirty = true;
}

void BasicShaderPass::SetLights(const DirectionalLight& directionalLight, const PointLight& pointLight, const SpotLight& spotLight)
{
    sceneLightData.directionalLight = directionalLight;
    sceneLightData.pointLight = pointLight;
    sceneLightData.spotLight = spotLight;
    sceneLightDataDirty = true;
}

void BasicShaderPass::SetEyePosition(const DirectX::XMFLOAT3& eyeWorldPosition)
{
    sceneLightData.eyeWorldPosition = eyeWorldPosition;
    sceneLightDataDirty = true;
}

void BasicShaderPass::UpdateObjectRenderData(ID3D11DeviceContext* immediateContext)
{
    if (!objectRenderDataDirty)
    {
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mapped;
    immediateContext->Map(objectRenderCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    *static_cast<ObjectRenderData*>(mapped.pData) = objectRenderData;
    immediateContext->Unmap(objectRenderCBuffer.Get(), 0);

    objectRenderDataDirty = false;
}

void BasicShaderPass::UpdateSceneLightData(ID3D11DeviceContext* immediateContext)
{
    if (!sceneLightDataDirty)
    {
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mapped;
    immediateContext->Map(sceneLightCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    *static_cast<SceneLightData*>(mapped.pData) = sceneLightData;
    immediateContext->Unmap(sceneLightCBuffer.Get(), 0);

    sceneLightDataDirty = false;
}
