struct VertexIn
{
    float4 posCS : SV_Position;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 color : SV_Target;
};

VertexOut PS_Main(VertexIn input)
{
    VertexOut output;
    output.color = input.color;
    return output;
}