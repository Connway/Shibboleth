struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

struct PixelOutputType
{
	float4 diffuse : SV_TARGET0;
	float4 specular : SV_TARGET1;
	float4 normal : SV_TARGET2;
	float4 position : SV_TARGET3;
	//float depth : SV_DEPTH;
};

SamplerState sampler;
Texture2D tex;

PixelOutputType PixelMain(PixelInputType input)
{
	PixelOutputType output;

	output.diffuse = tex.Sample(sampler, input.uv);

	output.specular = float4(1.0, 0.0, 0.0, 1.0);
	output.normal = input.normal;
	output.position = input.position;
	//output.depth = input.position.z / input.position.w;
	//output.depth = input.position.z;

	// Gamma correction: 255 * (Image/255)^(1/2.2)
	//output.diffuse.rgb = pow(output.diffuse.rgb, 1.0 / 2.2);

	return output;
}
