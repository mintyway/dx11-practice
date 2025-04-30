#pragma once

#include "Core/Shaders/ShaderPass/ShaderPassBase.h"
#include "Core/Light/Light.h"
#include "DirectXMath.h"

class TexturedHillsAndWavesShaderPass final : public ShaderPassBase
{
    struct RenderData
    {
        DirectX::XMFLOAT4X4 worldMatrix;
        DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
        DirectX::XMFLOAT4X4 wvpMatrix;
        DirectX::XMFLOAT4X4 uvMatrix;
        Material material;
    };

    struct LightData
    {
        DirectionalLight directionalLight;
        PointLight pointLight;
        SpotLight spotLight;
        alignas(16) DirectX::XMFLOAT3 eyePosition;
    };

    DECLARE_SHADER(TexturedHillsAndWavesShaderPass, ShaderPassBase)

public:
    TexturedHillsAndWavesShaderPass(ID3D11Device* device);
    ~TexturedHillsAndWavesShaderPass() override = default;

    void Bind(ID3D11DeviceContext* context) override;

    void UpdateCBuffer(ID3D11DeviceContext* context);

    void SetMatrix(DirectX::FXMMATRIX worldMatrix, DirectX::CXMMATRIX viewProjectionMatrix);
    void SetUVMatrix(DirectX::FXMMATRIX uvMatrix);
    void SetMaterial(const Material& material);

    void SetLights(const DirectionalLight& directionalLight, const PointLight& pointLight, const SpotLight& spotLight);
    void SetEyePosition(const DirectX::XMFLOAT3& eyePosition);

protected:
    void UpdateRenderData(ID3D11DeviceContext* context);
    void UpdateLightData(ID3D11DeviceContext* context);

    ComPtr<ID3D11Buffer> renderCBuffer;
    ComPtr<ID3D11Buffer> lightCBuffer;

    RenderData renderData{};
    LightData lightData{};
};
