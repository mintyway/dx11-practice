#include "BasicShader.h"

#include <d3dcompiler.h>
#include "Data/Path.h"
#include "Rendering/Vertex.h"

void BasicShader::Init(ID3D11Device* device)
{
    Super::Init(device);

    ComPtr<ID3DBlob> vertexShaderBlob;
    D3DReadFileToBlob(Path::GetShaderPath(L"basic_vs.cso").c_str(), &vertexShaderBlob);
    device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader);

    ComPtr<ID3DBlob> pixelShaderBlob;
    D3DReadFileToBlob(Path::GetShaderPath(L"basic_ps.cso").c_str(), &pixelShaderBlob);
    device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader);

    device->CreateInputLayout(Vertex::PN::Desc.data(), static_cast<UINT>(Vertex::PN::Desc.size()), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout);
}
