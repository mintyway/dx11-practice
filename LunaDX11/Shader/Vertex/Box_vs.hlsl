cbuffer ConstantBufferPerObject : register(b0)
{
    row_major float4x4 wvpMatrix;
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
    VertexOut output;
    output.posCS = mul(float4(input.posOS, 1.0f), wvpMatrix);
    output.color = input.color;
    return output;
}
