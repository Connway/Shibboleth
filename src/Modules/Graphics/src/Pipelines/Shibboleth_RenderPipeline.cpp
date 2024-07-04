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
#include "Shibboleth_GraphicsLogging.h"
#include "Shibboleth_RenderManager.h"
#include <Ptrs/Shibboleth_ManagerRef.h>
#include <Config/Shibboleth_Config.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::RenderPipeline)
	.classAttrs(
		Shibboleth::ConfigFileAttribute{ u8"graphics/render_pipelines" },
		Shibboleth::InitFromConfigAttribute{}
	)

	.serialize(&Shibboleth::RenderPipeline::Load)

	.var("render_data", &Shibboleth::RenderPipeline::_render_data)
	.var("stages", &Shibboleth::RenderPipeline::_stages)
	.var("update_groups", &Shibboleth::RenderPipeline::_update_groups)
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
		data = &_render_data.push(ref_def);

		if (!data->init(*ManagerRef<RenderManager>{})) {
			LogErrorGraphics("RenderPipeline::getOrAddRenderData: Failed to initialize '%s'.", ref_def.getReflectionInstance().getName());
			return nullptr;
		}
	}

	return data;
}

const IRenderPipelineData* RenderPipeline::getRenderData(const Refl::IReflectionDefinition& ref_def) const
{
	return const_cast<RenderPipeline*>(this)->getRenderData(ref_def);
}

IRenderPipelineData* RenderPipeline::getRenderData(const Refl::IReflectionDefinition& ref_def)
{
	const auto it = Gaff::Find(
		_render_data,
		ref_def,
		[](const IRenderPipelineData& data, const Refl::IReflectionDefinition& ref_def) -> bool
		{
			return &data.getReflectionDefinition() == &ref_def;
		}
	);

	return (it == _render_data.end()) ? nullptr : it.get();
}

const InstancedArray<IRenderPipelineData>& RenderPipeline::getRenderData(void) const
{
	return _render_data;
}

bool RenderPipeline::Load(const ISerializeReader& reader, RenderPipeline& object)
{
	GAFF_REF(reader, object);

	{
		const auto guard = reader.enterElementGuard(u8"render_data");

		if (!reader.isNull()) {
			Refl::IReflectionVar* const var = Refl::Reflection<RenderPipeline>::GetReflectionDefinition().getVar(Gaff::FNV1aHash32Const(u8"render_data"));
			GAFF_ASSERT(var);

			if (!var->load(reader, &object._render_data)) {
				return false;
			}
		}
	}

	const auto guard = reader.enterElementGuard(u8"stages");

	if (reader.isNull()) {
		return true;
	}

	if (!reader.isArray()) {
		// $TODO: Log error.
		return false;
	}

	static constexpr auto k_add_stage = [](const ISerializeReader& reader, RenderPipeline& object) -> bool
	{
		GAFF_ASSERT(reader.isString());

		const char8_t * const class_name = reader.readString();
		const Refl::IReflectionDefinition* const ref_def = GetApp().getReflectionManager().getReflection(Gaff::FNV1aHash64String(class_name));
		GAFF_ASSERT(ref_def->template hasInterface<IRenderPipelineStage>());

		if (ref_def) {
			object._stages.push(*ref_def);
			reader.freeString(class_name);
			return true;

		} else {
			// $TODO: Log error.
			reader.freeString(class_name);

			return false;
		}
	};

	bool success = true;

	reader.forEachInArray([&](int32_t) -> bool
	{
		int32_t group_count = 0;

		if (reader.isArray()) {
			reader.forEachInArray([&](int32_t) -> bool
			{
				if (k_add_stage(reader, object)) {
					++group_count;
				}

				return false;
			});

		} else if (reader.isString()) {
			if (k_add_stage(reader, object)) {
				++group_count;
			}

		} else {
			// $TODO: Log error.
			success = false;
		}

		if (group_count) {
			object._update_groups.emplace_back(group_count);
		}

		return false;
	});

	object._update_groups.shrink_to_fit();
	return success;
}

NS_END
