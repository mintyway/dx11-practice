#include "SharedTypes.hlsli"

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
