/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_MaterialComponent.h"
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_ECSAttributes.h>
#include <Shibboleth_ECSManager.h>

SHIB_REFLECTION_DEFINE_BEGIN_NEW(Material)
	.classAttrs(
		ECSClassAttribute(nullptr, "Graphics")
	)

	.setInstanceHash(MaterialComponentHash)

	.base< ECSComponentBaseShared<Material> >()

	.var("material", &Material::material)

	.var("textures_vertex", &Material::textures_vertex, OptionalAttribute())
	.var("textures_pixel", &Material::textures_pixel, OptionalAttribute())
	.var("textures_domain", &Material::textures_domain, OptionalAttribute())
	.var("textures_geometry", &Material::textures_geometry, OptionalAttribute())
	.var("textures_hull", &Material::textures_hull, OptionalAttribute())

	.var("samplers_vertex", &Material::samplers_vertex, OptionalAttribute())
	.var("samplers_pixel", &Material::samplers_pixel, OptionalAttribute())
	.var("samplers_domain", &Material::samplers_domain, OptionalAttribute())
	.var("samplers_geometry", &Material::samplers_geometry, OptionalAttribute())
	.var("samplers_hull", &Material::samplers_hull, OptionalAttribute())

	.ctor<>()
SHIB_REFLECTION_DEFINE_END_NEW(Material)

NS_SHIBBOLETH

static Gaff::Hash64 MaterialComponentHash(const Material& material, Gaff::Hash64 init)
{
	init = Gaff::FNV1aHash64T(material.material, init);

	// Textures
	for (const auto& texture : material.textures_vertex) {
		init = Gaff::FNV1aHash64T(texture.second, init);
	}

	for (const auto& texture : material.textures_pixel) {
		init = Gaff::FNV1aHash64T(texture.second, init);
	}

	for (const auto& texture : material.textures_domain) {
		init = Gaff::FNV1aHash64T(texture.second, init);
	}

	for (const auto& texture : material.textures_geometry) {
		init = Gaff::FNV1aHash64T(texture.second, init);
	}

	for (const auto& texture : material.textures_hull) {
		init = Gaff::FNV1aHash64T(texture.second, init);
	}

	// Samplers
	for (const auto& sampler : material.samplers_vertex) {
		init = Gaff::FNV1aHash64T(sampler.second, init);
	}

	for (const auto& sampler : material.samplers_pixel) {
		init = Gaff::FNV1aHash64T(sampler.second, init);
	}

	for (const auto& sampler : material.samplers_domain) {
		init = Gaff::FNV1aHash64T(sampler.second, init);
	}

	for (const auto& sampler : material.samplers_geometry) {
		init = Gaff::FNV1aHash64T(sampler.second, init);
	}

	for (const auto& sampler : material.samplers_hull) {
		init = Gaff::FNV1aHash64T(sampler.second, init);
	}

	return init;
}

NS_END
