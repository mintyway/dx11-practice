#pragma once

#include "Core/Data/CBufferData.h"

#include <d3d11.h>
#include <wrl/client.h>

#define DECLARE_SHADER(ClassName, ParentClassName)\
    friend class ShaderBase;\
    using Super = ParentClassName;\
    using ThisClass = ClassName;\
public:\
    ClassName(const ClassName&) = delete;\
    ClassName& operator=(const ClassName&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
    ClassName(ClassName&&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
    ClassName& operator=(ClassName&&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
private:

struct Material;

class ShaderBase
{
    friend class ShaderLibrary;

public:
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ShaderBase() = default;
    ShaderBase(const ShaderBase&) = default;
    ShaderBase(ShaderBase&&) = default;
    virtual ~ShaderBase() = default;

    ShaderBase& operator=(const ShaderBase&) = default;
    ShaderBase& operator=(ShaderBase&&) = default;

    void Bind(ID3D11DeviceContext* immediateContext);
    void UpdateCBuffer(ID3D11DeviceContext* immediateContext);

    void SetWorldMatrix(const XMFLOAT4X4& inWorldMatrix);
    void SetViewProjectionMatrix(const XMFLOAT4X4& inViewProjectionMatrix);
    void SetMaterial(const Material& material);

    void SetDirectionalLight(const DirectionalLight& directionalLight);
    void SetPointLight(const PointLight& pointLight);
    void SetSpotLight(const SpotLight& spotLight);
    void SetEyePosition(const XMFLOAT3& eyeWorldPosition);

protected:
    virtual void Init(ID3D11Device* device);

    void UpdateObjectRenderData(ID3D11DeviceContext* immediateContext);
    void UpdateSceneLightData(ID3D11DeviceContext* immediateContext);

    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;

    ComPtr<ID3D11Buffer> objectRenderCBuffer;
    ComPtr<ID3D11Buffer> sceneLightCBuffer;

    ObjectRenderData objectRenderData{};
    SceneLightData sceneLightData{};

    XMFLOAT4X4 viewProjectionMatrix{};

    bool objectRenderDataDirty = true;
    bool sceneLightDataDirty = true;
};
