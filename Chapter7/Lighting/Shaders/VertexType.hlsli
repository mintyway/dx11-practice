#pragma once

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
