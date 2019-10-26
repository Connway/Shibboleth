/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_RenderCommandSystem.h"
#include "Shibboleth_RenderManagerBase.h"
#include <Shibboleth_ECSComponentCommon.h>
#include <Shibboleth_ECSManager.h>
#include <Gleam_IncludeMatrix.h>

SHIB_REFLECTION_DEFINE(RenderCommandSystem)

NS_SHIBBOLETH

static bool FilterMaterial(const Material& material)
{
	const auto devices = material.material->getDevices();

	if (devices.empty()) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::IProgram* const program = material.material->getProgram(*devices[0]);
	const Gleam::IShader* const vert_shader = program->getAttachedShader(Gleam::IShader::SHADER_VERTEX);

	if (vert_shader) {
		const Gleam::ShaderReflection refl = vert_shader->getReflectionData();

		for (int32_t i = 0; i < refl.num_structured_buffers; ++i) {
			const auto& sb_refl = refl.structured_buffers[i];

			for (int32_t j = 0; j < sb_refl.num_vars; ++j) {
				if (sb_refl.vars[j].name == "_model_to_proj_matrix") {
					return true;
				}
			}
		}
	}

	return false;
}


SHIB_REFLECTION_CLASS_DEFINE_BEGIN(RenderCommandSystem)
	.BASE(ISystem)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(RenderCommandSystem)

bool RenderCommandSystem::init(void)
{
	ECSQuery query;

	query.addShared(_materials, Gaff::Func<bool (const Material&)>(&FilterMaterial));
	query.addShared(_models);

	query.add<Position>(_position);
	query.add<Rotation>(_rotation);
	query.add<Scale>(_scale);

	// Add callbacks for adding/removing buffers.
	auto add_func = Gaff::MemberFunc(this, &RenderCommandSystem::newArchetype);
	auto remove_func = Gaff::MemberFunc(this, &RenderCommandSystem::removedArchetype);

	query.addArchetypeCallbacks(std::move(add_func), std::move(remove_func));

	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	_res_mgr = &GetApp().getManagerTFast<ResourceManager>();
	_ecs_mgr = &GetApp().getManagerTFast<ECSManager>();
	_ecs_mgr->registerQuery(std::move(query));

	return true;
}

void RenderCommandSystem::update()
{
	// All arrays should be same size.
	const int32_t size = static_cast<int32_t>(_materials.size());

	for (int32_t i = 0; i < size; ++i) {
		_ecs_mgr->iterate<Position, Rotation, Scale>([&](EntityID id, const Position& pos, const Rotation& rot, const Scale& scale) -> void
		{
			GAFF_REF(id);

			glm::mat4x4 transform = glm::scale(glm::identity<glm::mat4x4>(), scale.value);
			transform *= glm::mat4_cast(rot.value);
			transform = glm::translate(transform, pos.value);

			// Write to instance buffer.
		},
		_position[i], _rotation[i], _scale[i]);
	}
}

void RenderCommandSystem::newArchetype(void)
{
	auto& program_buffers = _program_buffers.emplace_back();
	auto& buffer = _buffers.emplace_back();

	const auto& material = _materials.back()->material;
	const auto devices = material->getDevices();

	if (devices.empty()) {
		// $TODO: Log error.
		return;
	}

	const Gleam::IProgram* const program = material->getProgram(*devices[0]);
	const Gleam::IShader* const vert_shader = program->getAttachedShader(Gleam::IShader::SHADER_VERTEX);
	//size_t buffer_size = 0;

	if (vert_shader) {
		const Gleam::ShaderReflection refl = vert_shader->getReflectionData();

		for (int32_t i = 0; i < refl.num_structured_buffers; ++i) {
			if (refl.structured_buffers[i].name == "instance_data") {

				break;
			}
		}
	}

	const int32_t id = _next_id++;
	const U8String pb_name = U8String("RenderCommandSystem:ProgramBuffers:").append_sprintf("%i", id);
	const U8String b_name = U8String("RenderCommandSystem:Buffer:").append_sprintf("%i", id);

	program_buffers = _res_mgr->createResourceT<ProgramBuffersResource>(pb_name.data());
	buffer = _res_mgr->createResourceT<BufferResource>(b_name.data());

	const Gleam::IBuffer::BufferSettings settings = {
		nullptr,
		0, //size_t size = 0;
		0, //int32_t stride = 0;
		Gleam::IBuffer::BT_STRUCTURED_DATA,
		Gleam::IBuffer::MT_WRITE,
		true
	};

	if (!program_buffers->createProgramBuffers(_materials.back()->material->getDevices())) {
		// $TODO: Log error.
		return;
	}

	if (!buffer->createBuffer(_materials.back()->material->getDevices(), settings)) {
		// $TODO: Log error.
		return;
	}
}

void RenderCommandSystem::removedArchetype(int32_t index)
{
	GAFF_REF(index);
}

NS_END
