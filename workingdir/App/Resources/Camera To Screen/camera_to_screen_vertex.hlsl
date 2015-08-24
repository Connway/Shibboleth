struct VertexInputType
{
	float3 position : POSITION0;
	float2 uv : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

PixelInputType VertexMain(VertexInputType input)
{
	PixelInputType output;

	output.position = float4(input.position, 1.0);
	output.uv = input.uv;

	return output;
}
