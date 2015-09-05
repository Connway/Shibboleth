struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

PixelInputType VertexMain(uint VertexID : SV_VertexID)
{
	PixelInputType output;

	output.uv = float2((VertexID << 1) & 2, VertexID & 2);

	output.position = float4(
		output.uv * float2(2.0, -2.0) + float2(-1.0, 1.0),
		0.0, 1.0
	);

	return output;
}
