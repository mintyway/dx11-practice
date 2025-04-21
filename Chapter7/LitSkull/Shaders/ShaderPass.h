#pragma once
#include "Core/Light/Light.h"
#include "Core/Shaders/ShaderPass/ShaderPassBase.h"

class ShaderPass final : public ShaderPassBase
{
    struct alignas(16) RenderData
    {
        DirectX::XMFLOAT4X4 worldMatrix;
        DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
        DirectX::XMFLOAT4X4 wvpMatrix;
        Material material;
    };

    struct LightData
    {
        DirectionalLight directionalLights[3];
        DirectX::XMFLOAT3 eyePosition;
        UINT activeCount;
    };

    DECLARE_SHADER(ShaderPass, ShaderPassBase)

public:
    ShaderPass(ID3D11Device* device);
    ~ShaderPass();

    void Bind(ID3D11DeviceContext* immediateContext) override;

    void UpdateCBuffer(ID3D11DeviceContext* immediateContext);

    void SetMatrix(DirectX::FXMMATRIX worldMatrix, DirectX::CXMMATRIX viewMatrix, DirectX::CXMMATRIX projectionMatrix);
    void SetMaterial(const Material& material);
    
    void SetDirectionalLights(const std::array<DirectionalLight, 3>& directionalLights);
    void SetEyePosition(const DirectX::XMFLOAT3& eyePosition);
    void SetActiveDirectionalLightCount(UINT activeCount);

private:
    void UpdateRenderData(ID3D11DeviceContext* immediateContext);
    void UpdateLightData(ID3D11DeviceContext* immediateContext);

    ComPtr<ID3D11Buffer> renderDataCBuffer;
    ComPtr<ID3D11Buffer> lightDataCBuffer;

    RenderData renderData{};
    LightData lightData{};

    bool renderDataDirty = true;
    bool lightDataDirty = true;
};
