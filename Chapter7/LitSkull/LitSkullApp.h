#pragma once

#include "Core/Engine/SphericalCamera.h"
#include "Core/Light/Light.h"
#include "Core/Rendering/Submesh.h"

#include <array>
#include <unordered_map>

#include "Core/Rendering/Vertex.h"

class ShaderPass;

class LitSkullApp final : public SphericalCamera
{
    enum class ObjectType : uint8_t
    {
        Grid,
        Box,
        Sphere,
        Cylinder,
        Skull
    };

    struct RenderAsset
    {
        Material material;
        Submesh submesh;
    };

    DECLARE_ENGINE(LitSkullApp, SphericalCamera)

public:
    LitSkullApp();
    ~LitSkullApp() override = default;

    bool Init(HINSTANCE inInstanceHandle) override;
    LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
    void OnResize() override;

protected:
    void Update(float deltaSeconds) override;
    void Render() override;

    void RenderObject(DirectX::XMMATRIX worldMatrix, const Material& material, const Submesh& submesh);

private:
    void InitShaderPass();
    void InitGeometryBuffer();
    void InitSkullBuffer(std::vector<Vertex::PN>& inoutVertices, std::vector<UINT>& inoutIndices);
    void InitShapeBuffer(std::vector<Vertex::PN>& inoutVertices, std::vector<UINT>& inoutIndices);

    std::unique_ptr<ShaderPass> shaderPass;

    DirectX::XMFLOAT3 eyePosition{};
    DirectX::XMFLOAT4X4 viewMatrix{};
    DirectX::XMFLOAT4X4 projectionMatrix{};

    const std::array<DirectionalLight, 3> DirectionalLights;
    UINT activeLightCount = static_cast<UINT>(DirectionalLights.size());

    ComPtr<ID3D11Buffer> skullVertexBuffer;
    ComPtr<ID3D11Buffer> skullIndexBuffer;

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    DirectX::XMFLOAT4X4 skullWorldMatrix{};
    DirectX::XMFLOAT4X4 boxWorldMatrix{};
    DirectX::XMFLOAT4X4 gridWorldMatrix{};
    std::array<DirectX::XMFLOAT4X4, 10> sphereWorldMatrices{};
    std::array<DirectX::XMFLOAT4X4, 10> cylinderMatrices{};

    std::unordered_map<ObjectType, RenderAsset> RenderAssetMap;
};
