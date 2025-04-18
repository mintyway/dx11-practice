#pragma once

#include "Core/Shaders/ShaderType/ShaderBase.h"
#include "Core/Light/Light.h"

class BasicShader final : public ShaderBase
{
    struct ObjectRenderData
    {
        XMFLOAT4X4 worldMatrix;
        XMFLOAT4X4 worldInverseTransposeMatrix;
        XMFLOAT4X4 wvpMatrix;
        Material material;
    };

    struct SceneLightData
    {
        DirectionalLight directionalLight;
        PointLight pointLight;
        SpotLight spotLight;
        alignas(16) XMFLOAT3 eyeWorldPosition;
    };

    DECLARE_SHADER(BasicShader, ShaderBase)

public:
    BasicShader(ID3D11Device* device);
    ~BasicShader() override = default;

    void Bind(ID3D11DeviceContext* immediateContext) override;

    void UpdateCBuffer(ID3D11DeviceContext* immediateContext);

    void SetMatrix(FXMMATRIX worldMatrix, CXMMATRIX viewProjectionMatrix);
    void SetMaterial(const Material& material);

    void SetLights(const DirectionalLight& directionalLight, const PointLight& pointLight, const SpotLight& spotLight);
    void SetEyePosition(const XMFLOAT3& eyeWorldPosition);

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
