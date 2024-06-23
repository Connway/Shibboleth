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

#pragma once

#include "Resources/Shibboleth_SamplerStateResource.h"
#include "Resources/Shibboleth_MaterialResource.h"
#include "Resources/Shibboleth_TextureResource.h"
#include "Model/Shibboleth_ModelResource.h"

NS_SHIBBOLETH

class ITransformProvider;

struct TextureInstanceData final
{
	using TextureMap = VectorMap< HashString32<>, ResourcePtr<TextureResource> >;

	TextureMap vertex;
	TextureMap pixel;
	TextureMap domain;
	TextureMap geometry;
	TextureMap hull;
};

struct SamplerInstanceData final
{
	using SamplerMap = VectorMap< HashString32<>, ResourcePtr<SamplerStateResource> >;

	SamplerMap vertex;
	SamplerMap pixel;
	SamplerMap domain;
	SamplerMap geometry;
	SamplerMap hull;
};

struct MaterialInstanceData final
{
	ResourcePtr<MaterialResource> material;
	TextureInstanceData textures;
	SamplerInstanceData samplers;
};

struct ModelInstanceHandle final
{
	Gaff::Hash64 instance_hash;
	Gaff::Hash64 bucket_hash;
	const ITransformProvider* transform_provider = nullptr;
};

struct ModelInstanceData final
{
	// $TODO: Add support for automatically sizing this according to the number of meshes in the model resource.
	Vector<MaterialInstanceData> material_data{ GRAPHICS_ALLOCATOR };
	ResourcePtr<ModelResource> model;

	// $TODO: Fixed size.
	int32_t instances_per_page = 64;

	ModelInstanceHandle createInstanceHandle(const ITransformProvider& tform_provider) const;

	void calculateInstanceAndBucketHash(Gaff::Hash64& bucket_hash, Gaff::Hash64& instance_hash) const;
	Gaff::Hash64 calculateInstanceHash(void) const;
	Gaff::Hash64 calculateBucketHash(void) const;
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::MaterialInstanceData)
SHIB_REFLECTION_DECLARE(Shibboleth::TextureInstanceData)
SHIB_REFLECTION_DECLARE(Shibboleth::SamplerInstanceData)
SHIB_REFLECTION_DECLARE(Shibboleth::ModelInstanceData)
