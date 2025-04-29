#include "Core/Shaders/HLSL/LightingCommon.hlsli"
#include "Core/Shaders/HLSL/LightingFunction.hlsli"

cbuffer ConstantBufferPerObject : register(b0)
{
    row_major float4x4 worldMatrix;
    row_major float4x4 worldInverseTransposeMatrix;
    row_major float4x4 wvpMatrix;
    Material material;
};

cbuffer ConstantBufferPerFrame : register(b1)
{
    DirectionalLight directionalLight;
    PointLight pointLight;
    SpotLight spotLight;
    float3 eyeWorldPosition;
}

struct VertexIn
{
    float3 positionOS : POSITION;
    float3 normalOS : NORMAL;
};

struct VertexOut
{
    float3 positionWS : POSITION;
    float4 positionCS : SV_Position;
    float3 normalWS : NORMAL;
};

VertexOut VS_Main(VertexIn input)
{
    VertexOut output;
    output.positionWS = mul(float4(input.positionOS, 1.0f), worldMatrix).xyz;
    output.normalWS = mul(input.normalOS, (float3x3)worldInverseTransposeMatrix);
    output.positionCS = mul(float4(input.positionOS, 1.0f), wvpMatrix);
    return output;
}

float4 PS_Main(VertexOut input) : SV_Target
{
    input.normalWS = normalize(input.normalWS);

    const float3 toEyeWS = normalize(eyeWorldPosition - input.positionWS);

    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 outAmbient, outDiffuse, outSpecular;

    ComputeDirectionalLight(material, directionalLight, input.normalWS, toEyeWS, outAmbient, outDiffuse, outSpecular);
    ambient += outAmbient;
    diffuse += outDiffuse;
    specular += outSpecular;

    ComputePointLight(material, pointLight, input.positionWS, input.normalWS, toEyeWS, outAmbient, outDiffuse, outSpecular);
    ambient += outAmbient;
    diffuse += outDiffuse;
    specular += outSpecular;

    ComputeSpotLight(material, spotLight, input.positionWS, input.normalWS, toEyeWS, outAmbient, outDiffuse, outSpecular);
    ambient += outAmbient;
    diffuse += outDiffuse;
    specular += outSpecular;

    float4 litColor = ambient + diffuse + specular;
    litColor.a = material.diffuse.a;
    return litColor;
}
