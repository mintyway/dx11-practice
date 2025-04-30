#pragma once

#include "Core/Shaders/ShaderPass/ShaderPassBase.h"
#include "Core/Light/Light.h"
#include "DirectXMath.h"

class BasicShaderPass final : public ShaderPassBase
{
    struct ObjectRenderData
    {
        DirectX::XMFLOAT4X4 worldMatrix;
        DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
        DirectX::XMFLOAT4X4 wvpMatrix;
        Material material;
    };

    struct SceneLightData
    {
        DirectionalLight directionalLight;
        PointLight pointLight;
        SpotLight spotLight;
        alignas(16) DirectX::XMFLOAT3 eyeWorldPosition;
    };

    DECLARE_SHADER(BasicShaderPass, ShaderPassBase)

public:
    BasicShaderPass(ID3D11Device* device);
    ~BasicShaderPass() override = default;

    void Bind(ID3D11DeviceContext* context) override;

    void UpdateCBuffer(ID3D11DeviceContext* immediateContext);

    void SetMatrix(DirectX::FXMMATRIX worldMatrix, DirectX::CXMMATRIX viewProjectionMatrix);
    void SetMaterial(const Material& material);

    void SetLights(const DirectionalLight& directionalLight, const PointLight& pointLight, const SpotLight& spotLight);
    void SetEyePosition(const DirectX::XMFLOAT3& eyeWorldPosition);

protected:
    void UpdateObjectRenderData(ID3D11DeviceContext* immediateContext);
    void UpdateSceneLightData(ID3D11DeviceContext* immediateContext);

    ComPtr<ID3D11Buffer> objectRenderCBuffer;
    ComPtr<ID3D11Buffer> sceneLightCBuffer;

    ObjectRenderData objectRenderData{};
    SceneLightData sceneLightData{};

    bool objectRenderDataDirty = true;
    bool sceneLightDataDirty = true;
};
