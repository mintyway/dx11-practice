#pragma once

#include <d3d11.h>
#include <string>
#include <vector>
#include <wrl/client.h>

#define DECLARE_SHADER(ClassName, ParentClassName)\
    using Super = ParentClassName;\
    using ThisClass = ClassName;\
public:\
    ClassName(const ClassName&) = default;\
    ClassName& operator=(const ClassName&) = default; /*NOLINT(bugprone-macro-parentheses)*/\
    ClassName(ClassName&&) = default; /*NOLINT(bugprone-macro-parentheses)*/\
    ClassName& operator=(ClassName&&) = default; /*NOLINT(bugprone-macro-parentheses)*/\
private:

struct Material;

class ShaderPassBase
{
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    ShaderPassBase(ID3D11Device* device, const std::wstring& vertexShaderName, const std::wstring& pixelShaderName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDescs);
    ShaderPassBase(const ShaderPassBase&) = default;
    ShaderPassBase(ShaderPassBase&&) = default;
    virtual ~ShaderPassBase() = default;

    ShaderPassBase& operator=(const ShaderPassBase&) = default;
    ShaderPassBase& operator=(ShaderPassBase&&) = default;

    virtual void Bind(ID3D11DeviceContext* context);

protected:
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;
};
