struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

struct PixelOutputType
{
	float4 final_pixel : SV_TARGET0;
};

SamplerState sample;

Texture2D diffuse;
Texture2D specular;
Texture2D normal;
Texture2D position;
Texture2D depth;

PixelOutputType PixelMain(PixelInputType input)
{
	PixelOutputType output;

	output.final_pixel = diffuse.Sample(sample, input.uv);

	return output;
}
