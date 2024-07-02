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

#include "Pipelines/Shibboleth_RenderPipeline.h"
#include <Config/Shibboleth_Config.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::RenderPipeline)
	.classAttrs(
		Shibboleth::ConfigFileAttribute(u8"graphics/render_pipelines"),
		Shibboleth::InitFromConfigAttribute()
	)

	.var("stages", &Shibboleth::RenderPipeline::_stages)
SHIB_REFLECTION_DEFINE_END(Shibboleth::RenderPipeline)

NS_SHIBBOLETH

Error RenderPipeline::init(RenderManager& /*render_mgr*/)
{
	const Refl::ReflectionDefinition<RenderPipeline>& ref_def = Refl::Reflection<RenderPipeline>::GetReflectionDefinition();
	const InitFromConfigAttribute* const config_attr = ref_def.getClassAttr<InitFromConfigAttribute>();
	GAFF_ASSERT(config_attr);

	const Error error = config_attr->loadConfig(this, ref_def);

	if (error.hasError()) {
		// $TODO: Log error.
	}

	return error;
}

Vector<const IRenderPipelineStage*> RenderPipeline::getRenderStages(const Refl::IReflectionDefinition& ref_def) const
{
	Vector<const IRenderPipelineStage*> out;

	for (const IRenderPipelineStage& stage : _stages) {
		const Refl::IReflectionDefinition& stage_ref_def = stage.getReflectionDefinition();

		if (&stage_ref_def == &ref_def || stage_ref_def.hasInterface(ref_def)) {
			out.emplace_back(&stage);
		}
	}

	return out;
}

Vector<IRenderPipelineStage*> RenderPipeline::getRenderStages(const Refl::IReflectionDefinition& ref_def)
{
	Vector<IRenderPipelineStage*> out;

	for (IRenderPipelineStage& stage : _stages) {
		const Refl::IReflectionDefinition& stage_ref_def = stage.getReflectionDefinition();

		if (&stage_ref_def == &ref_def || stage_ref_def.hasInterface(ref_def)) {
			out.emplace_back(&stage);
		}
	}

	return out;
}

const IRenderPipelineStage* RenderPipeline::getRenderStagePtr(const Refl::IReflectionDefinition& ref_def) const
{
	return const_cast<RenderPipeline*>(this)->getRenderStagePtr(ref_def);
}

IRenderPipelineStage* RenderPipeline::getRenderStagePtr(const Refl::IReflectionDefinition& ref_def)
{
	for (IRenderPipelineStage& stage : _stages) {
		const Refl::IReflectionDefinition& stage_ref_def = stage.getReflectionDefinition();

		if (&stage_ref_def == &ref_def || stage_ref_def.hasInterface(ref_def)) {
			return &stage;
		}
	}

	return nullptr;
}

const IRenderPipelineStage& RenderPipeline::getRenderStage(const Refl::IReflectionDefinition& ref_def) const
{
	return const_cast<RenderPipeline*>(this)->getRenderStage(ref_def);
}

IRenderPipelineStage& RenderPipeline::getRenderStage(const Refl::IReflectionDefinition& ref_def)
{
	IRenderPipelineStage* const stage = getRenderStagePtr(ref_def);
	GAFF_ASSERT(stage);

	return *stage;
}

IRenderPipelineData* RenderPipeline::getOrAddRenderData(const Refl::IReflectionDefinition& ref_def)
{
	IRenderPipelineData* data = getRenderData(ref_def);

	if (!data) {
		ProxyAllocator allocator = GRAPHICS_ALLOCATOR;
		data = ref_def.template createT<IRenderPipelineData>(allocator);
	}

	return data;
}

const IRenderPipelineData* RenderPipeline::getRenderData(const Refl::IReflectionDefinition& ref_def) const
{
	return const_cast<RenderPipeline*>(this)->getRenderData(ref_def);
}

IRenderPipelineData* RenderPipeline::getRenderData(const Refl::IReflectionDefinition& ref_def)
{
	const auto it = Gaff::FindSorted(
		_render_data,
		ref_def,
		[](const UniquePtr<IRenderPipelineData>& data, const Refl::IReflectionDefinition& ref_def) -> bool
		{
			return &data->getReflectionDefinition() == &ref_def;
		}
	);

	return (it == _render_data.end()) ? nullptr : it->get();
}

const Vector< UniquePtr<IRenderPipelineData> >& RenderPipeline::getRenderData(void) const
{
	return _render_data;
}

NS_END
