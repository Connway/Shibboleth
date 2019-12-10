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
#include "Shibboleth_CameraComponent.h"
#include <Shibboleth_ECSComponentCommon.h>
#include <Shibboleth_ECSManager.h>
#include <Gleam_IncludeMatrix.h>

SHIB_REFLECTION_DEFINE(RenderCommandSystem)

NS_SHIBBOLETH

//static bool FilterMaterial(const Material& material)
//{
//	const auto devices = material.material->getDevices();
//
//	if (devices.empty()) {
//		// $TODO: Log error.
//		return false;
//	}
//
//	const Gleam::IProgram* const program = material.material->getProgram(*devices[0]);
//	const Gleam::IShader* const vert_shader = program->getAttachedShader(Gleam::IShader::SHADER_VERTEX);
//
//	if (vert_shader) {
//		const Gleam::ShaderReflection refl = vert_shader->getReflectionData();
//
//		for (const auto& sb_refl : refl.structured_buffers) {
//			for (const auto& var_refl : sb_refl.vars) {
//				if (var_refl.name == "_model_to_proj_matrix") {
//					return true;
//				}
//			}
//		}
//	}
//
//	return false;
//}


SHIB_REFLECTION_CLASS_DEFINE_BEGIN(RenderCommandSystem)
	.BASE(ISystem)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(RenderCommandSystem)

bool RenderCommandSystem::init(void)
{
	ECSQuery camera_query;
	ECSQuery object_query;

	object_query.addShared(_buffer_count, true);
	object_query.addShared(_materials/*, Gaff::Func<bool (const Material&)>(&FilterMaterial)*/);
	object_query.addShared(_models);

	object_query.add<Position>(_position);
	object_query.add<Rotation>(_rotation);
	object_query.add<Scale>(_scale);

	camera_query.add<Position>(_camera_position);
	camera_query.add<Rotation>(_camera_rotation);
	camera_query.add<Camera>(_camera);

	// Add callbacks for adding/removing buffers.
	auto add_func = Gaff::MemberFunc(this, &RenderCommandSystem::newArchetype);
	auto remove_func = Gaff::MemberFunc(this, &RenderCommandSystem::removedArchetype);

	object_query.addArchetypeCallbacks(std::move(add_func), std::move(remove_func));

	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	_res_mgr = &GetApp().getManagerTFast<ResourceManager>();
	_ecs_mgr = &GetApp().getManagerTFast<ECSManager>();
	_ecs_mgr->registerQuery(std::move(object_query));
	_ecs_mgr->registerQuery(std::move(camera_query));

	return true;
}

void RenderCommandSystem::update()
{
	// All arrays should be same size.
	const int32_t num_objects = static_cast<int32_t>(_materials.size());
	const int32_t num_cameras = static_cast<int32_t>(_camera.size());

	for (int32_t camera_index = 0; camera_index < num_cameras; ++camera_index) {
		_ecs_mgr->iterate<Position, Rotation, Camera>([&](EntityID, const Position& cam_pos, const Rotation& cam_rot, const Camera& camera) -> void
		{
			const Gleam::IWindow* const window = _render_mgr->getWindow(camera.display_tag);

			if (!window) {
				return;
			}

			const auto devices = _render_mgr->getDevicesByTag(camera.display_tag);

			const glm::mat4x4 projection = glm::perspectiveFovLH(
				camera.GetVerticalFOVDegrees(),
				static_cast<float>(window->getWidth()),
				static_cast<float>(window->getHeight()),
				camera.z_near, camera.z_far
			);

			const glm::mat4x4 camera_transform = glm::transpose(glm::translate(glm::mat4_cast(cam_rot.value), cam_pos.value));
			const glm::mat4x4 final_camera = projection * camera_transform;

			for (int32_t i = 0; i < num_objects; ++i) {
				Vector<void*> buffers(devices->size(), nullptr);
				int32_t stride = 0;
				int32_t index = 0;

				for (int32_t j = 0; j < static_cast<int32_t>(devices->size()); ++j) {
					Gleam::IRenderDevice& rd = *devices->at(j);
					Gleam::IBuffer* const buffer = _instance_data[i].buffer->getBuffer(rd);

					buffers[j] = buffer->map(rd);
					stride = buffer->getStride();
				}

				_ecs_mgr->iterate<Position, Rotation, Scale>([&](EntityID, const Position& obj_pos, const Rotation& obj_rot, const Scale& obj_scale) -> void
				{
					glm::mat4x4 transform = glm::scale(glm::identity<glm::mat4x4>(), obj_scale.value);
					transform *= glm::mat4_cast(obj_rot.value);
					transform = glm::translate(transform, obj_pos.value);

					// Write to instance buffer.
					for (void* buffer : buffers) {
						glm::mat4x4* const matrix = reinterpret_cast<glm::mat4x4*>(reinterpret_cast<int8_t*>(buffer) + (stride * index) + _instance_data[i].model_to_proj_offset);
						*matrix = final_camera * transform;
					}

					++index;
				},
				_position[i], _rotation[i], _scale[i]);
			}
		},
		_camera_position[camera_index], _camera_rotation[camera_index], _camera[camera_index]);
	}
}

void RenderCommandSystem::newArchetype(void)
{
	InstanceData& instance_data = _instance_data.emplace_back();

	auto* const material = _materials.back()->material.get();
	auto* const model = _models.back()->value.get();
	const uint32_t id = _next_id++;

	const U8String pb_name = U8String("RenderCommandSystem:ProgramBuffers:").append_sprintf("%i", id);
	const U8String b_name = U8String("RenderCommandSystem:Buffer:").append_sprintf("%i", id);

	instance_data.program_buffers = _res_mgr->createResourceT<ProgramBuffersResource>(pb_name.data());
	instance_data.buffer = _res_mgr->createResourceT<BufferResource>(b_name.data());

	Vector<IResource*> resources{ material, model };

	const int32_t buffer_count = (_buffer_count.back()) ? _buffer_count.back()->value : 0;

	_res_mgr->registerCallback(resources, Gaff::Func<void (const Vector<IResource*>&)>([this, &instance_data, material, buffer_count](const Vector<IResource*>&) -> void {
		processNewArchetypeMaterial(instance_data, *material, buffer_count);
	}));
}

void RenderCommandSystem::removedArchetype(int32_t index)
{
	GAFF_REF(index);
}

void RenderCommandSystem::processNewArchetypeMaterial(InstanceData& instance_data, const MaterialResource& material, int32_t buffer_count)
{
	const auto devices = material.getDevices();

	if (devices.empty()) {
		// $TODO: Log error.
		return;
	}

	const Gleam::IProgram* const program = material.getProgram(*devices[0]);
	const Gleam::IShader* const vert_shader = program->getAttachedShader(Gleam::IShader::SHADER_VERTEX);
	size_t instance_data_size =  0;

	if (vert_shader) {
		const Gleam::ShaderReflection refl = vert_shader->getReflectionData();

		for (const auto& sb_refl : refl.structured_buffers) {
			if (sb_refl.name == "instance_data") {
				instance_data_size = sb_refl.size_bytes;

				for (const auto& var_refl : sb_refl.vars) {
					if (var_refl.name == "_model_to_proj_matrix") {
						instance_data.model_to_proj_offset = static_cast<int32_t>(var_refl.start_offset);
						break;
					}
				}

				break;
			}
		}
	}

	const Gleam::IBuffer::BufferSettings settings = {
		nullptr,
		instance_data_size * static_cast<size_t>((buffer_count > 0) ? buffer_count : 64), // size
		static_cast<int32_t>(instance_data_size), // stride
		Gleam::IBuffer::BT_STRUCTURED_DATA,
		Gleam::IBuffer::MT_WRITE,
		true
	};

	if (!instance_data.program_buffers->createProgramBuffers(_materials.back()->material->getDevices())) {
		// $TODO: Log error.
		return;
	}

	if (!instance_data.buffer->createBuffer(_materials.back()->material->getDevices(), settings)) {
		// $TODO: Log error.
		return;
	}

	for (Gleam::IRenderDevice* rd : instance_data.program_buffers->getDevices()) {
		Gleam::IShaderResourceView* const srv = _render_mgr->createShaderResourceView();

		if (!srv->init(*rd, instance_data.buffer->getBuffer(*rd))) {
			// $TODO: Log error.
			SHIB_FREET(srv, GetAllocator());
		}

		instance_data.program_buffers->getProgramBuffer(*rd)->addResourceView(Gleam::IShader::SHADER_VERTEX, srv);
		instance_data.buffer_srvs[rd].reset(srv);
	}
}

NS_END
