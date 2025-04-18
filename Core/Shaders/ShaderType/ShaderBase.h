#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <wrl/client.h>

using namespace DirectX;

#define DECLARE_SHADER(ClassName, ParentClassName)\
    friend class ShaderBase;\
    using Super = ParentClassName;\
    using ThisClass = ClassName;\
public:\
    ClassName(const ClassName&) = default;\
    ClassName& operator=(const ClassName&) = default; /*NOLINT(bugprone-macro-parentheses)*/\
    ClassName(ClassName&&) = default; /*NOLINT(bugprone-macro-parentheses)*/\
    ClassName& operator=(ClassName&&) = default; /*NOLINT(bugprone-macro-parentheses)*/\
private:

struct Material;

class ShaderBase
{
public:
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ShaderBase(ID3D11Device* device, const std::wstring& vertexShaderName, const std::wstring& pixelShaderName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDescs);
    ShaderBase(const ShaderBase&) = default;
    ShaderBase(ShaderBase&&) = default;
    virtual ~ShaderBase() = default;

    ShaderBase& operator=(const ShaderBase&) = default;
    ShaderBase& operator=(ShaderBase&&) = default;

    virtual void Bind(ID3D11DeviceContext* immediateContext);

protected:
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;
};
