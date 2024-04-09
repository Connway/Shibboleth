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
#include "Resources/Shibboleth_ModelResource.h"

NS_SHIBBOLETH

class ITransformProvider;

struct TextureData final
{
	using TextureMap = VectorMap< HashString32<>, ResourcePtr<TextureResource> >;

	TextureMap vertex;
	TextureMap pixel;
	TextureMap domain;
	TextureMap geometry;
	TextureMap hull;
};

struct SamplerData final
{
	using SamplerMap = VectorMap< HashString32<>, ResourcePtr<SamplerStateResource> >;

	SamplerMap vertex;
	SamplerMap pixel;
	SamplerMap domain;
	SamplerMap geometry;
	SamplerMap hull;
};

struct MaterialData final
{
	ResourcePtr<MaterialResource> material;
	TextureData textures;
	SamplerData samplers;
};

struct ModelData final
{
	ResourcePtr<ModelResource> model;
	// $TODO: Add support for automatically sizing this according to the number of meshes in the model resource.
	Vector<MaterialData> material_data{ GRAPHICS_ALLOCATOR };

	int32_t instances_per_page = 64;
};

class IModelStageRegistration
{
public:
	struct ModelInstanceHandle final
	{
		Gaff::Hash64 bucket_hash{ 0 };
		Gaff::Hash64 instance_hash{ 0 };
		const ITransformProvider* provider = nullptr;

		bool isValid(void) const { return bucket_hash.getHash() != 0 && instance_hash.getHash() != 0; }
	};

	virtual ~IModelStageRegistration(void) {}

	virtual ModelInstanceHandle registerModel(const ModelData& data, const ITransformProvider& transform_provider) = 0;
	virtual void unregisterModel(ModelInstanceHandle handle) = 0;
};

NS_END

NS_HASHABLE
	GAFF_CLASS_HASHABLE(Shibboleth::IModelStageRegistration);
NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::MaterialData)
SHIB_REFLECTION_DECLARE(Shibboleth::TextureData)
SHIB_REFLECTION_DECLARE(Shibboleth::SamplerData)
SHIB_REFLECTION_DECLARE(Shibboleth::ModelData)
