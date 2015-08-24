struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

struct PixelOutputType
{
	float4 diffuse : SV_TARGET0;
};

SamplerState sample;
Texture2D tex;

PixelOutputType PixelMain(PixelInputType input)
{
	PixelOutputType output;
	output.diffuse = tex.Sample(sample, input.uv);
	return output;
}
