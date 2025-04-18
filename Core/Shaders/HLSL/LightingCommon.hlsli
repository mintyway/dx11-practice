#ifndef LIGHTING_COMMON_HLSLI
#define LIGHTING_COMMON_HLSLI

struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 direction;
};

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 position;
    float range;

    float3 attenuation;
};

struct SpotLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 position;
    float range;

    float3 direction;
    float spot;

    float3 attenuation;
};

struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 reflect;
};

#endif
