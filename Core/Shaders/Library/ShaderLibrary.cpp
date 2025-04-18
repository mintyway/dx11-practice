#include "ShaderLibrary.h"

#include "Shaders/ShaderType/BasicShader.h"

void ShaderLibrary::LoadAll(ID3D11Device* device)
{
    if (bInitialized)
    {
        return;
    }

    shaders["Basic"] = std::make_shared<BasicShader>();
    shaders["Basic"]->Init(device);

    bInitialized = true;
}

std::shared_ptr<ShaderBase> ShaderLibrary::Get(const std::string& shaderName)
{
    if (!shaders.contains(shaderName))
    {
        return nullptr;
    }

    return shaders[shaderName];
}
