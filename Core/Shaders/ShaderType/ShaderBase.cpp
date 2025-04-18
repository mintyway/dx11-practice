#include "ShaderBase.h"

#include "Data/Path.h"

#include <d3dcompiler.h>

ShaderBase::ShaderBase(ID3D11Device* device, const std::wstring& vertexShaderName, const std::wstring& pixelShaderName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDescs)
{
    const std::wstring extension = L".cso";

    ComPtr<ID3DBlob> vertexShaderBlob;
    D3DReadFileToBlob(Path::GetShaderPath(vertexShaderName + extension).c_str(), &vertexShaderBlob);
    device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader);

    ComPtr<ID3DBlob> pixelShaderBlob;
    D3DReadFileToBlob(Path::GetShaderPath(pixelShaderName + extension).c_str(), &pixelShaderBlob);
    device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader);

    device->CreateInputLayout(inputElementDescs.data(), static_cast<UINT>(inputElementDescs.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout);
}

void ShaderBase::Bind(ID3D11DeviceContext* immediateContext)
{
    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    immediateContext->IASetInputLayout(inputLayout.Get());
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
