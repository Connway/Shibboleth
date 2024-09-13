/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include "Model/Shibboleth_ModelInstanceData.h"
#include <Gleam_ShaderResourceView.h>
#include <Gleam_RasterState.h>
#include <Gleam_Texture.h>
#include <Gleam_Mesh.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::MaterialInstanceData)
	.template ctor<>()

	.var("material", &Type::material)
	.var("textures", &Type::textures)
	.var("samplers", &Type::samplers)
SHIB_REFLECTION_DEFINE_END(Shibboleth::MaterialInstanceData)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::TextureInstanceData)
	.template ctor<>()

	.var("vertex", &Type::::vertex)
	.var("pixel", &Type::::pixel)
	.var("domain", &Type::::domain)
	.var("geometry", &Type::::geometry)
	.var("hull", &Type::::hull)
SHIB_REFLECTION_DEFINE_END(Shibboleth::TextureInstanceData)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::SamplerInstanceData)
	.template ctor<>()

	.var("vertex", &Type::::vertex)
	.var("pixel", &Type::::pixel)
	.var("domain", &Type::::domain)
	.var("geometry", &Type::::geometry)
	.var("hull", &Type::::hull)
SHIB_REFLECTION_DEFINE_END(Shibboleth::SamplerInstanceData)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ModelInstanceData)
	.template ctor<>()

	.var("raster_state", &Type::::raster_state)
	.var("model", &Type::::model)
	.var("material_data", &Type::::material_data)
	.var("instances_per_page", &Type::::instances_per_page, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Shibboleth::ModelInstanceData)

NS_SHIBBOLETH

ModelInstanceHandle ModelInstanceData::createInstanceHandle(const ITransformProvider& tform_provider) const
{
	ModelInstanceHandle handle;
	handle.transform_provider = &tform_provider;

	calculateInstanceAndBucketHash(handle.bucket_hash, handle.instance_hash);

	return handle;
}

void ModelInstanceData::calculateInstanceAndBucketHash(Gaff::Hash64& bucket_hash, Gaff::Hash64& instance_hash) const
{
	static constexpr auto AddResourcesToHash = []<class Map>(const Map& resource_map, Gaff::Hash64& bucket_hash) -> void
	{
		for (const auto& entry : resource_map) {
			bucket_hash = Gaff::FNV1aHash64T(entry.second.get(), bucket_hash);
		}
	};

	bucket_hash = Gaff::FNV1aHash64T(model.get());
	bucket_hash = Gaff::FNV1aHash64T(raster_state.get());

	for (const MaterialInstanceData& data : material_data) {
		bucket_hash = Gaff::FNV1aHash64T(data.material.get(), bucket_hash);

		// Samplers
		AddResourcesToHash(data.samplers.vertex, bucket_hash);
		AddResourcesToHash(data.samplers.pixel, bucket_hash);
		AddResourcesToHash(data.samplers.domain, bucket_hash);
		AddResourcesToHash(data.samplers.geometry, bucket_hash);
		AddResourcesToHash(data.samplers.hull, bucket_hash);
	}

	instance_hash = bucket_hash;

	for (const MaterialInstanceData& data : material_data) {
		// Only hashing texture data for instance, as we can use a texture array to simplify the number of render calls needed.
		AddResourcesToHash(data.textures.vertex, instance_hash);
		AddResourcesToHash(data.textures.pixel, instance_hash);
		AddResourcesToHash(data.textures.domain, instance_hash);
		AddResourcesToHash(data.textures.geometry, instance_hash);
		AddResourcesToHash(data.textures.hull, instance_hash);
	}

	// $TODO: This is temporary until I refactor textures to use a texture array.
	bucket_hash = instance_hash;
}

Gaff::Hash64 ModelInstanceData::calculateInstanceHash(void) const
{
	Gaff::Hash64 bucket_hash, instance_hash;

	calculateInstanceAndBucketHash(bucket_hash, instance_hash);

	return instance_hash;
}

Gaff::Hash64 ModelInstanceData::calculateBucketHash(void) const
{
	Gaff::Hash64 bucket_hash, instance_hash;

	calculateInstanceAndBucketHash(bucket_hash, instance_hash);

	return bucket_hash;
}

NS_END
