#include "Core/Shaders/HLSL/LightingCommon.hlsli"
#include "Core/Shaders/HLSL/LightingFunction.hlsli"

cbuffer RenderData : register(b0)
{
    row_major float4x4 worldMatrix;
    row_major float4x4 worldInverseTransposeMatrix;
    row_major float4x4 wvpMatrix;
    row_major float4x4 texTransform;
    Material material;
    bool useTexture;
}

cbuffer LightData : register(b1)
{
    DirectionalLight directionalLights[3];
    float3 eyePositionWS;
    uint activeCount;
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
    output.positionCS = mul(float4(input.positionOS, 1.0f), wvpMatrix);
    output.normalWS = mul(input.normalOS, (float3x3)worldInverseTransposeMatrix);
    output.tex = mul(float4(input.tex, 0.0f, 1.0f), texTransform).xy;
    return output;
}

float4 PS_Main(VertexOut input) : SV_Target
{
    input.normalWS = normalize(input.normalWS);

    const float3 toEyeWS = normalize(eyePositionWS - input.positionWS);

    float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (useTexture)
    {
        texColor = diffuseMap.Sample(diffuseMapSampler, input.tex);
    }

    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 litColor = texColor;
    for (uint i = 0; i < activeCount; ++i)
    {
        float4 outAmbient, outDiffuse, outSpecular;
        ComputeDirectionalLight(material, directionalLights[i], input.normalWS, toEyeWS, outAmbient, outDiffuse, outSpecular);
        ambient += outAmbient;
        diffuse += outDiffuse;
        specular += outSpecular;

        litColor = texColor * (ambient + diffuse) + specular;
    }

    litColor.a = material.diffuse.a * texColor.a;
    return litColor;
}
