#include "Core/Shaders/HLSL/LightingCommon.hlsli"
#include "Core/Shaders/HLSL/LightingFunction.hlsli"

cbuffer ConstantBufferPerObject : register(b0)
{
    row_major float4x4 worldMatrix;
    row_major float4x4 worldInverseTransposeMatrix;
    row_major float4x4 wvpMatrix;
    row_major float4x4 uvMatrix;
    Material material;
    bool useTexture = true;
};

cbuffer ConstantBufferPerFrame : register(b1)
{
    DirectionalLight directionalLights[3];
    float3 eyePosition;
    uint activeCount;

    float4 fogColor;
    float fogStart;
    float fogRange;
    bool useFog;
}

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSampler : register(s0);

struct VertexIn
{
    float3 positionOS : POSITION;
    float3 normalOS : NORMAL;
    float2 tex : TEXCOORD;
};

struct VertexOut
{
    float3 positionWS : POSITION;
    float4 positionCS : SV_Position;
    float3 normalWS : NORMAL;
    float2 tex : TEXCOORD;
};

VertexOut VS_Main(VertexIn input)
{
    VertexOut output;
    output.positionWS = mul(float4(input.positionOS, 1.0f), worldMatrix).xyz;
    output.normalWS = mul(input.normalOS, (float3x3)worldInverseTransposeMatrix);
    output.positionCS = mul(float4(input.positionOS, 1.0f), wvpMatrix);
    output.tex = mul(float4(input.tex, 0.0f, 1.0f), uvMatrix).xy;
    return output;
}

float4 PS_Main(VertexOut input) : SV_Target
{
    input.normalWS = normalize(input.normalWS);

    const float3 toEye = eyePosition - input.positionWS;
    const float distanceToEye = length(toEye);
    const float3 toEyeDirection = toEye / distanceToEye;

    float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (useTexture)
    {
        texColor = diffuseMap.Sample(diffuseMapSampler, input.tex);
    }

    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 outAmbient, outDiffuse, outSpecular;

    for (uint i = 0; i < activeCount; ++i)
    {
        ComputeDirectionalLight(material, directionalLights[i], input.normalWS, toEyeDirection, outAmbient, outDiffuse, outSpecular);
        ambient += outAmbient;
        diffuse += outDiffuse;
        specular += outSpecular;
    }

    float4 litColor = (ambient + diffuse) * texColor + specular;

    if (useFog)
    {
        const float fogFactor = saturate((distanceToEye - fogStart) / fogRange);
        litColor = lerp(litColor, fogColor, fogFactor);
    }

    litColor.a = material.diffuse.a * texColor.a;
    return litColor;
}
