cbuffer MatrixBuffer
{
	matrix world_matrix;
	matrix view_matrix;
	matrix proj_matrix;
};

struct VertexInputType
{
	float3 position : POSITION0;
	float3 normal: NORMAL0;
	float3 tangent : TANGENT0;
	float3 bitangent : BINORMAL0;
	float2 uv : TEXCOORD0;
	uint4 blend_indices : BLENDINCIES0;
	float4 blend_weights : BLENDWEIGHTS0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	//float4 normal : NORMAL0;
	//float3 tangent : TANGENT0;
	//float3 bitangent : BINORMAL0;
	float2 uv : TEXCOORD0;
};

PixelInputType VertexMain(VertexInputType input)
{
	PixelInputType output;

	output.position = mul(world_matrix, float4(input.position, 1.0));
	output.position = mul(view_matrix, output.position);
	output.position = mul(proj_matrix, output.position);
	//output.normal = float4(input.normal, 1.0);
	output.uv = input.uv;

	return output;
}
