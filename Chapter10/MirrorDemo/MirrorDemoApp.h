#pragma once

#include <array>

#include "Core/Engine/SphericalCamera.h"
#include "Core/Light/Light.h"
#include "Core/Rendering/Submesh.h"
#include "Core/Utilities/Waves.h"

class MirrorDemoShaderPass;

class MirrorDemoApp final : public SphericalCamera
{
    DECLARE_ENGINE(MirrorDemoApp, SphericalCamera)

public:
    MirrorDemoApp();
    ~MirrorDemoApp() override;

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;

protected:
    virtual void Update(float deltaSeconds) override;
    virtual void Render() override;

    void RenderObject(ID3D11Buffer* vertexBufferPtr, ID3D11Buffer* indexBufferPtr, DirectX::XMMATRIX worldMatrix, DirectX::CXMMATRIX viewProjectionMatrix, DirectX::CXMMATRIX uvMatrix, ID3D11ShaderResourceView* diffuseMapSRV, const Material& material, Submesh submesh);

private:
    void CreateGeometry();
    void CreateSkullGeometry();
    void CreateRoomGeometry();

    void InitTexture();

    std::unique_ptr<MirrorDemoShaderPass> shaderPass;
    ComPtr<ID3D11SamplerState> samplerState;

    DirectX::XMFLOAT4X4 viewMatrix;
    DirectX::XMFLOAT4X4 projectionMatrix;
    DirectX::XMFLOAT3 eyePosition;

    std::array<DirectionalLight, 3> directionalLights;

    ComPtr<ID3D11Buffer> skullVertexBuffer;
    ComPtr<ID3D11Buffer> skullIndexBuffer;
    DirectX::XMFLOAT4X4 skullWorldMatrix;
    DirectX::XMFLOAT3 skullTranslation;
    Material skullMaterial;
    Submesh skullSubmesh;

    ComPtr<ID3D11Buffer> roomVertexBuffer;
    DirectX::XMFLOAT4X4 roomWorldMatrix;
    Material roomMaterial;
    ComPtr<ID3D11ShaderResourceView> floorDiffuseMapSrv;
    ComPtr<ID3D11ShaderResourceView> mirrorDiffuseMapSrv;
    Material mirrorMaterial;

    ComPtr<ID3D11ShaderResourceView> wallDiffuseMapSrv;
};
