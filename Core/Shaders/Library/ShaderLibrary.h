#pragma once

#include <d3d11.h>
#include <string>
#include <memory>
#include <unordered_map>

class ShaderBase;

class ShaderLibrary
{
public:
    static void LoadAll(ID3D11Device* device);
    static std::shared_ptr<ShaderBase> Get(const std::string& shaderName);

private:
    static inline std::unordered_map<std::string, std::shared_ptr<ShaderBase>> shaders;
    static inline bool bInitialized = false;
};
