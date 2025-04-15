cbuffer ConstantBufferPerObject : register(b0)
{
    row_major float4x4 wvpMatrix;
};

cbuffer TimeBuffer : register(b1)
{
    float time;
};

struct VertexIn
{
    float3 posOS : POSITION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 posCS : SV_Position;
    float4 color : COLOR;
};

VertexOut VS_Main(VertexIn input)
{
    input.posOS.xy += 0.5f * sin(input.posOS.x) * sin(3.0f * time);
    input.posOS.z *= 0.6f + 0.4f * sin(2.0f * time);

    VertexOut output;
    output.posCS = mul(float4(input.posOS, 1.0f), wvpMatrix);
    output.color = input.color;
    return output;
}
