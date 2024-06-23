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
#include <Gleam_Texture.h>
#include <Gleam_Mesh.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::MaterialInstanceData)
	.template ctor<>()

	.var("material", &Shibboleth::MaterialInstanceData::material)
	.var("textures", &Shibboleth::MaterialInstanceData::textures)
	.var("samplers", &Shibboleth::MaterialInstanceData::samplers)
SHIB_REFLECTION_DEFINE_END(Shibboleth::MaterialInstanceData)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::TextureInstanceData)
	.template ctor<>()

	.var("vertex", &Shibboleth::TextureInstanceData::vertex)
	.var("pixel", &Shibboleth::TextureInstanceData::pixel)
	.var("domain", &Shibboleth::TextureInstanceData::domain)
	.var("geometry", &Shibboleth::TextureInstanceData::geometry)
	.var("hull", &Shibboleth::TextureInstanceData::hull)
SHIB_REFLECTION_DEFINE_END(Shibboleth::TextureInstanceData)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::SamplerInstanceData)
	.template ctor<>()

	.var("vertex", &Shibboleth::SamplerInstanceData::vertex)
	.var("pixel", &Shibboleth::SamplerInstanceData::pixel)
	.var("domain", &Shibboleth::SamplerInstanceData::domain)
	.var("geometry", &Shibboleth::SamplerInstanceData::geometry)
	.var("hull", &Shibboleth::SamplerInstanceData::hull)
SHIB_REFLECTION_DEFINE_END(Shibboleth::SamplerInstanceData)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ModelInstanceData)
	.template ctor<>()

	.var("model", &Shibboleth::ModelInstanceData::model)
	.var("material_data", &Shibboleth::ModelInstanceData::material_data)
	.var("instances_per_page", &Shibboleth::ModelInstanceData::instances_per_page, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Shibboleth::ModelInstanceData)

NS_SHIBBOLETH

void ModelInstanceData::getInstanceAndBucketHash(Gaff::Hash64& bucket_hash, Gaff::Hash64& instance_hash) const
{
	static constexpr auto AddResourcesToHash = []<class Map>(const Map& resource_map, Gaff::Hash64& bucket_hash) -> void
	{
		for (const auto& entry : resource_map) {
			bucket_hash = Gaff::FNV1aHash64T(entry.second.get(), bucket_hash);
		}
	};

	if (instance_hash_cache != k_init_hash64 && bucket_hash_cache != k_init_hash64) {
		instance_hash = instance_hash_cache;
		bucket_hash = bucket_hash_cache;
		return;
	}

	bucket_hash_cache = Gaff::FNV1aHash64T(data.model.get());

	for (const MaterialData& material_data : data.material_data) {
		bucket_hash_cache = Gaff::FNV1aHash64T(material_data.material.get(), bucket_hash);
		resource_list.emplace_back(material_data.material.get());

		// Samplers
		AddResourcesToHash(material_data.samplers.vertex, bucket_hash);
		AddResourcesToHash(material_data.samplers.pixel, bucket_hash);
		AddResourcesToHash(material_data.samplers.domain, bucket_hash);
		AddResourcesToHash(material_data.samplers.geometry, bucket_hash);
		AddResourcesToHash(material_data.samplers.hull, bucket_hash);
	}

	instance_hash_cache = bucket_hash_cache;

	for (const MaterialData& material_data : data.material_data) {
		// Only hashing texture data for instance, as we can use a texture array to simplify the number of render calls needed.
		AddResourcesToHash(material_data.textures.vertex, instance_hash_cache);
		AddResourcesToHash(material_data.textures.pixel, instance_hash_cache);
		AddResourcesToHash(material_data.textures.domain, instance_hash_cache);
		AddResourcesToHash(material_data.textures.geometry, instance_hash_cache);
		AddResourcesToHash(material_data.textures.hull, instance_hash_cache);
	}

	// $TODO: This is temporary until I refactor textures to use a texture array.
	bucket_hash_cache = instance_hash_cache;

	instance_hash = instance_hash_cache;
	bucket_hash = bucket_hash_cache;
}

Gaff::Hash64 ModelInstanceData::getInstanceHash(void) const
{
	calculateInstanceAndBucketHash(bucket_hash_cache, instance_hash_cache);
	return instance_hash_cache;
}

Gaff::Hash64 ModelInstanceData::getBucketHash(void) const
{
	calculateInstanceAndBucketHash(bucket_hash_cache, instance_hash_cache);
	return bucket_hash_cache;
}

NS_END
