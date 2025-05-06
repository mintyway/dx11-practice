#pragma once

#include <span>

#include "Core/Shaders/ShaderPass/ShaderPassBase.h"
#include "Core/Light/Light.h"
#include "DirectXMath.h"

class BlendDemoShaderPass final : public ShaderPassBase
{
    DECLARE_SHADER(BlendDemoShaderPass, ShaderPassBase)

public:
    struct alignas(16) RenderData
    {
        DirectX::XMFLOAT4X4 worldMatrix;
        DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
        DirectX::XMFLOAT4X4 wvpMatrix;
        DirectX::XMFLOAT4X4 uvMatrix;
        Material material;
    };

    struct alignas(16) LightData
    {
        DirectionalLight directionalLight[3];
        DirectX::XMFLOAT3 eyePosition;
        UINT activeDirectionalLightCount = 3;
    };

    BlendDemoShaderPass(ID3D11Device* device);
    ~BlendDemoShaderPass() override = default;

    void Bind(ID3D11DeviceContext* context) override;

    void UpdateCBuffer(ID3D11DeviceContext* context);

    void SetMatrix(DirectX::FXMMATRIX worldMatrix, DirectX::CXMMATRIX viewProjectionMatrix);
    void SetUVMatrix(DirectX::FXMMATRIX uvMatrix);
    void SetMaterial(const Material& material);

    void SetLights(std::span<const DirectionalLight, 3> directionalLights);
    void SetEyePosition(const DirectX::XMFLOAT3& eyePosition);
    void SetActiveDirectionalLightCount(UINT count);

protected:
    void UpdateRenderData(ID3D11DeviceContext* context);
    void UpdateLightData(ID3D11DeviceContext* context);

    ComPtr<ID3D11Buffer> renderCBuffer;
    ComPtr<ID3D11Buffer> lightCBuffer;

    RenderData renderData{};
    LightData lightData{};
};
