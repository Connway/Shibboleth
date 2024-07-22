//cbuffer InstanceBuffer
//{
//	matrix model_to_proj_matrix;
//};

//cbuffer CameraBuffer
//{
//	//matrix view_matrix;
//	//matrix proj_matrix;
//};

// $TODO: Convert to cbuffer.
struct InstanceData
{
	matrix model_to_proj_matrix;
};

struct VertexInputType
{
	float3 position : POSITION0;
	float3 normal: NORMAL0;
	float3 tangent : TANGENT0;
	float3 bitangent : BINORMAL0;
	float2 uv : TEXCOORD0;
	//uint4 blend_indices : BLENDINDICES0;
	//float4 blend_weights : BLENDWEIGHTS0;
	uint instance_id : SV_InstanceID;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL0;
	//float3 tangent : TANGENT0;
	//float3 bitangent : BINORMAL0;
	float2 uv : TEXCOORD0;
};

// This always comes last
StructuredBuffer<InstanceData> instance_data;

PixelInputType VertexMain(VertexInputType input)
{
	PixelInputType output;

	output.position = mul(
		instance_data[input.instance_id].model_to_proj_matrix,
		float4(input.position, 1.0)
	);

	output.normal = float4(input.normal, 0.0);
	output.uv = input.uv;

	return output;
}
