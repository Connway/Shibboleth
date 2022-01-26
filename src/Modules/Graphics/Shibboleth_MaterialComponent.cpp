/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

NS_SHIBBOLETH
	static Gaff::Hash64 MaterialComponentHash(const Material& material, Gaff::Hash64 init);
NS_END

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::Material)
	.classAttrs(
		Shibboleth::ECSClassAttribute(nullptr, u8"Graphics")
	)

	.setInstanceHash(Shibboleth::MaterialComponentHash)

	.base< Shibboleth::ECSComponentBaseShared<Shibboleth::Material> >()

	.var("material", &Shibboleth::Material::material)

	.var("textures_vertex", &Shibboleth::Material::textures_vertex, Shibboleth::OptionalAttribute())
	.var("textures_pixel", &Shibboleth::Material::textures_pixel, Shibboleth::OptionalAttribute())
	.var("textures_domain", &Shibboleth::Material::textures_domain, Shibboleth::OptionalAttribute())
	.var("textures_geometry", &Shibboleth::Material::textures_geometry, Shibboleth::OptionalAttribute())
	.var("textures_hull", &Shibboleth::Material::textures_hull, Shibboleth::OptionalAttribute())

	.var("samplers_vertex", &Shibboleth::Material::samplers_vertex, Shibboleth::OptionalAttribute())
	.var("samplers_pixel", &Shibboleth::Material::samplers_pixel, Shibboleth::OptionalAttribute())
	.var("samplers_domain", &Shibboleth::Material::samplers_domain, Shibboleth::OptionalAttribute())
	.var("samplers_geometry", &Shibboleth::Material::samplers_geometry, Shibboleth::OptionalAttribute())
	.var("samplers_hull", &Shibboleth::Material::samplers_hull, Shibboleth::OptionalAttribute())

	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::Material)

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
