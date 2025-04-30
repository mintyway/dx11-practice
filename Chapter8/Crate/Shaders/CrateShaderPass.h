#pragma once

#include "Core/Light/Light.h"
#include "Core/Shaders/ShaderPass/ShaderPassBase.h"

class CrateShaderPass final : public ShaderPassBase
{
    struct alignas(16) RenderData
    {
        DirectX::XMFLOAT4X4 worldMatrix;
        DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
        DirectX::XMFLOAT4X4 wvpMatrix;
        DirectX::XMFLOAT4X4 textureTransformMatrix;
        Material material;
        bool useTexture;
    };

    struct alignas(16) LightData
    {
        DirectionalLight directionalLights[3];
        DirectX::XMFLOAT3 eyePosition;
        UINT activeCount;
    };

    DECLARE_SHADER(CrateShaderPass, ShaderPassBase)

public:
    CrateShaderPass(ID3D11Device* device);
    ~CrateShaderPass() override;

    void Bind(ID3D11DeviceContext* context) override;

    void UpdateCBuffer(ID3D11DeviceContext* immediateContext);

    void SetMatrix(DirectX::FXMMATRIX worldMatrix, DirectX::CXMMATRIX viewMatrix, DirectX::CXMMATRIX projectionMatrix);
    void SetTextureTransformMatrix(const DirectX::XMFLOAT4X4& transformMatrix);
    void SetMaterial(const Material& material);
    void SetUseTexture(bool newUseTexture);

    void SetDirectionalLights(const std::array<DirectionalLight, 3>& directionalLights);
    void SetEyePosition(const DirectX::XMFLOAT3& eyePosition);
    void SetActiveDirectionalLightCount(UINT activeCount);

    static void SetDiffuseMap(ID3D11DeviceContext* context, ID3D11ShaderResourceView* diffuseMapSRV);

private:
    void UpdateRenderData(ID3D11DeviceContext* immediateContext);
    void UpdateLightData(ID3D11DeviceContext* immediateContext);

    ComPtr<ID3D11Buffer> renderDataCBuffer;
    ComPtr<ID3D11Buffer> lightDataCBuffer;
    ComPtr<ID3D11SamplerState> samplerState;

    RenderData renderData{};
    LightData lightData{};

    bool renderDataDirty = true;
    bool lightDataDirty = true;
};
