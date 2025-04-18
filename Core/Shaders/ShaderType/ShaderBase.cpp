#include "ShaderBase.h"

#include <cstring>

void ShaderBase::Bind(ID3D11DeviceContext* immediateContext)
{
    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    immediateContext->IASetInputLayout(inputLayout.Get());
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D11Buffer* cBuffers[] = {objectRenderCBuffer.Get(), sceneLightCBuffer.Get()};
    immediateContext->VSSetConstantBuffers(0, 2, cBuffers);
    immediateContext->PSSetConstantBuffers(0, 2, cBuffers);
}

void ShaderBase::UpdateCBuffer(ID3D11DeviceContext* immediateContext)
{
    UpdateObjectRenderData(immediateContext);
    UpdateSceneLightData(immediateContext);
}

void ShaderBase::SetWorldMatrix(const XMFLOAT4X4& inWorldMatrix)
{
    objectRenderData.worldMatrix = inWorldMatrix;
    XMStoreFloat4x4(&objectRenderData.worldInverseTransposeMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&inWorldMatrix))));
    objectRenderDataDirty = true;
}

void ShaderBase::SetViewProjectionMatrix(const XMFLOAT4X4& inViewProjectionMatrix)
{
    viewProjectionMatrix = inViewProjectionMatrix;
    objectRenderDataDirty = true;
}

void ShaderBase::SetMaterial(const Material& material)
{
    objectRenderData.material = material;
    objectRenderDataDirty = true;
}

void ShaderBase::SetDirectionalLight(const DirectionalLight& directionalLight)
{
    sceneLightData.directionalLight = directionalLight;
    sceneLightDataDirty = true;
}

void ShaderBase::SetPointLight(const PointLight& pointLight)
{
    sceneLightData.pointLight = pointLight;
    sceneLightDataDirty = true;
}

void ShaderBase::SetSpotLight(const SpotLight& spotLight)
{
    sceneLightData.spotLight = spotLight;
    sceneLightDataDirty = true;
}

void ShaderBase::SetEyePosition(const XMFLOAT3& eyeWorldPosition)
{
    sceneLightData.eyeWorldPosition = eyeWorldPosition;
    sceneLightDataDirty = true;
}

void ShaderBase::Init(ID3D11Device* device)
{
    const CD3D11_BUFFER_DESC objectRenderCBufferDesc(sizeof(ObjectRenderData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&objectRenderCBufferDesc, nullptr, &objectRenderCBuffer);

    const CD3D11_BUFFER_DESC sceneLightDataCBufferDesc(sizeof(SceneLightData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    device->CreateBuffer(&sceneLightDataCBufferDesc, nullptr, &sceneLightCBuffer);
}

void ShaderBase::UpdateObjectRenderData(ID3D11DeviceContext* immediateContext)
{
    if (!objectRenderDataDirty)
    {
        return;
    }

    XMStoreFloat4x4(&objectRenderData.wvpMatrix, XMLoadFloat4x4(&objectRenderData.worldMatrix) * XMLoadFloat4x4(&viewProjectionMatrix));

    D3D11_MAPPED_SUBRESOURCE mapped;
    immediateContext->Map(objectRenderCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    *static_cast<ObjectRenderData*>(mapped.pData) = objectRenderData;
    immediateContext->Unmap(objectRenderCBuffer.Get(), 0);

    objectRenderDataDirty = false;
}

void ShaderBase::UpdateSceneLightData(ID3D11DeviceContext* immediateContext)
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
