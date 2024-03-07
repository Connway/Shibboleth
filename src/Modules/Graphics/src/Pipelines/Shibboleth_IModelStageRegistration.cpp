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

#include "Pipelines/Shibboleth_IModelStageRegistration.h"
#include <Gleam_ShaderResourceView.h>
#include <Gleam_Texture.h>
#include <Gleam_Mesh.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::MaterialData)
	.template ctor<>()

	.var("material", &Shibboleth::MaterialData::material)
	.var("textures", &Shibboleth::MaterialData::textures)
	.var("samplers", &Shibboleth::MaterialData::samplers)
SHIB_REFLECTION_DEFINE_END(Shibboleth::MaterialData)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::TextureData)
	.template ctor<>()

	.var("vertex", &Shibboleth::TextureData::vertex)
	.var("pixel", &Shibboleth::TextureData::pixel)
	.var("domain", &Shibboleth::TextureData::domain)
	.var("geometry", &Shibboleth::TextureData::geometry)
	.var("hull", &Shibboleth::TextureData::hull)
SHIB_REFLECTION_DEFINE_END(Shibboleth::TextureData)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::SamplerData)
	.template ctor<>()

	.var("vertex", &Shibboleth::SamplerData::vertex)
	.var("pixel", &Shibboleth::SamplerData::pixel)
	.var("domain", &Shibboleth::SamplerData::domain)
	.var("geometry", &Shibboleth::SamplerData::geometry)
	.var("hull", &Shibboleth::SamplerData::hull)
SHIB_REFLECTION_DEFINE_END(Shibboleth::SamplerData)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ModelData)
	.template ctor<>()

	.var("model", &Shibboleth::ModelData::model)
	.var("material_data", &Shibboleth::ModelData::material_data)
	.var("instances_per_page", &Shibboleth::ModelData::instances_per_page, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Shibboleth::ModelData)
